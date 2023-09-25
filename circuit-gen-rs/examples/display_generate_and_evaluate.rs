//! Run with eg `cargo run --example display_generate_and_evaluate --features="eval_plain"`
//!
//! NOTE: this is quite slow! You should probably avoid high resolution, and/or doing many evaluations.
//!
//! It is double uses:
//! - it CAN be used to test evaluation: eg ``
//! - it CAN also be used to write new Circuits to file; eg to update the tests in various repos
//!   eg `cargo run --features="eval_plain" --example display_generate_and_evaluate -- --width=400 --height=288 --output-path=display_message_640x360_2digits.skcd.postcard.bin --nb-evals=0`
//!     - previously width: 1080 / 2 = 640; now we crop a bit so 800 / 2 = 400
//!     - previously height: based on 16/9 ratio: 640 * 9 / 16 = 360
//!       but unused pixels at top and bottom(no digit, and never a watermark) so we crop also vertically
//!       360 * 0.80 (because digits were drawn with 0.1 offset and padding) = 288
//!   or `cargo run --features="eval_plain" --example display_generate_and_evaluate -- --width=590 --height=50 --is-pinpad --watermark-msg= --output-path=display_pinpad_590x50.skcd.postcard.bin --nb-evals=0`
//!

use circuit_types_rs::Circuit;
use circuit_types_rs::EvaluatorInputsType;
use circuit_types_rs::GarblerInputsType;
use clap::Parser;
use rand::distributions::Uniform;
use rand::prelude::Distribution;
use rand::thread_rng;

use lib_circuits_rs::generate_display_circuit;
use png_utils::write_png;

/// NOTE: the default is a "message" circuit; with a small resolution of 120x52
#[derive(Parser, Debug)]
#[clap(author, version, about, long_about = None)]
struct Args {
    /// For reference in repo /pallets/pallets/ocw-circuits/src/lib.rs:
    /// const DEFAULT_MESSAGE_WIDTH: u32 = 1280 / 2;
    /// const DEFAULT_PINPAD_WIDTH: u32 = 590;
    #[clap(long, required = false, default_value_t = 120)]
    width: u32,

    /// For reference in repo /pallets/pallets/ocw-circuits/src/lib.rs:
    /// const DEFAULT_MESSAGE_HEIGHT: u32 = 720 / 2;
    /// const DEFAULT_PINPAD_HEIGHT: u32 = 50;
    #[clap(long, required = false, default_value_t = 52)]
    height: u32,

    /// If set, the serialized Circuit will be written at this path
    #[clap(long, required = false)]
    output_path: Option<String>,

    #[clap(long, default_value_t = false)]
    is_pinpad: bool,

    #[clap(long, default_value = "test\nmessage")]
    watermark_msg: String,

    // /// The digits; ie the OTP or pinpad
    // // TODO(clap 4) https://stackoverflow.com/questions/73240901/how-to-get-clap-to-process-a-single-argument-with-multiple-values-without-having
    // #[clap(
    //     long,
    //     multiple = true,
    //     required = true,
    //     use_value_delimiter = true,
    //     value_delimiter = ','
    // )]
    // digits: Vec<u8>,
    /// How many eval() we will combine
    /// Reminder: each segment have a 50% chance to be displayed at each eval()
    /// So typically using 10 evals means almost all of the segments will be displayed
    #[clap(long, required = false, default_value_t = 1)]
    nb_evals: u64,
}

/// cf /lib-garble-rs/lib-garble-rs/src/lib.rs
#[derive(Debug)]
enum InterstellarError {
    GarblerInputsInvalidBufLength,
    GarblerInputs7SegmentsNotMod7,
    GarblerInputs7SegmentsWrongLength,
    NotAValid7Segment { digit: usize },
}

/// cf /lib-garble-rs/lib-garble-rs/src/garble.rs
type EvaluatorInput = u8;
type GarblerInput = u8;

/// copy-pasted from repo `lib-garble-rs` and adapted for NON-garbled Circuit
/// WARNING/NOTE: IF you update the logic here(eg new kind of input), you MUST also update it in repo `lib-garble-rs`
///
/// Prepare the `garbler_inputs`; it contains both:
/// - the watermark(ie the message)
/// - the 7 segments digits
/// NOTE: this is ONLY applicable to "display circuits"
///
/// # Errors
///
/// Will return en error when:
/// - "digits" contains value outside the valid 7 segments range [0-9]
/// - the inputs(ie "digits") length do not match what the circuit "garb" expects
///   eg if "garb" expects 14 bits of `garbler_input` for  7 segments -> digits.len() == 2
// TODO(interstellar) randomize 7 segs(then replace "garbler_input_segments")
// TODO(interstellar) the number of digits DEPENDS on the config!
fn garbled_display_circuit_prepare_garbler_inputs(
    circ: &Circuit,
    digits: &[u8],
    watermark_text: &str,
) -> Result<Vec<GarblerInput>, InterstellarError> {
    // Those are splitted into:
    // - "buf" gate (cf Verilog "rndswitch.v"; and correspondingly lib_garble/src/packmsg/packmsg_utils.cpp PrepareInputLabels);
    //    it MUST always be 0 else the 7 segments will not work as expected = 1 bit
    // - the segments to display: 7 segments * "nb of digits in the message" = 7 * N bits
    // - the watermark; one bit per pixel in the final display = width * height bits
    //
    // prepare using the correct garbler_inputs total length(in BITS)
    // ie simply sum the length of each GarblerInput
    let display_config = circ.get_config().unwrap();
    let mut garbler_inputs = Vec::with_capacity(
        display_config
            .garbler_inputs
            .iter()
            .fold(0, |acc, e| acc + e.length as usize),
    );
    for garbler_input in &display_config.garbler_inputs {
        match garbler_input.r#type {
            GarblerInputsType::Buf => {
                if garbler_input.length != 1 {
                    return Err(InterstellarError::GarblerInputsInvalidBufLength);
                }

                garbler_inputs.push(0u8);
            }
            GarblerInputsType::SevenSegments => {
                if garbler_input.length % 7 != 0 {
                    return Err(InterstellarError::GarblerInputs7SegmentsNotMod7);
                }
                if garbler_input.length as usize != digits.len() * 7 {
                    return Err(InterstellarError::GarblerInputs7SegmentsWrongLength);
                }

                // TODO handle digits_to_segments_bits? if needed: could probably add `lib-garble-rs` as a dev dep?
                // let mut segments_inputs = segments::digits_to_segments_bits(digits)
                //     .map_err(|e| InterstellarError::NotAValid7Segment { digit: e.number })?;
                let mut segments_inputs = vec![1; garbler_input.length as usize];

                garbler_inputs.append(&mut segments_inputs);
            }
            GarblerInputsType::Watermark => {
                // TODO handle watermark? if needed: could probably add `lib-garble-rs` as a dev dep?
                // let mut watermark_inputs = watermark::new_watermark(
                //     display_config.width,
                //     display_config.height,
                //     watermark_text,
                // )
                // .map_err(|err| InterstellarError::WatermarkError {
                //     msg: err.to_string(),
                // })?;
                let mut watermark_inputs =
                    vec![0; display_config.width as usize * display_config.height as usize];
                garbler_inputs.append(&mut watermark_inputs);
            }
        }
    }

    Ok(garbler_inputs)
}

/// copy-pasted from repo `lib-garble-rs` and adapted for NON-garbled Circuit
/// WARNING/NOTE: IF you update the logic here(eg new kind of input), you MUST also update it in repo `lib-garble-rs`
///
/// Like `garbled_display_circuit_prepare_garbler_inputs` but for the client-side(ie Evaluator)
/// Initialize a Vec for the "to be randomized each eval loop" evaluator inputs
///
/// # Errors
///
/// # Panics
///
/// TODO! If the given circuit if NOT a "display circuit" it will panic instead of properly passing to the client
fn prepare_evaluator_inputs(circ: &Circuit) -> Result<Vec<EvaluatorInput>, InterstellarError> {
    let display_config = circ.get_config().unwrap();
    let mut evaluator_inputs = Vec::with_capacity(
        display_config
            .evaluator_inputs
            .iter()
            .fold(0, |acc, e| acc + e.length as usize),
    );

    for evaluator_input in &display_config.evaluator_inputs {
        match evaluator_input.r#type {
            EvaluatorInputsType::Rnd => {
                let mut inputs_0 = vec![0; evaluator_input.length as usize];
                evaluator_inputs.append(&mut inputs_0);
            }
            _ => todo!("prepare_evaluator_inputs: only Rnd supported for now"),
        }
    }

    Ok(evaluator_inputs)
}

fn main() {
    let args = Args::parse();

    let (digits_bboxes, digits) = if !args.is_pinpad {
        let digits_bboxes = vec![
            // first digit bbox --------------------------------------------
            0.1625_f32, 0.0_f32, 0.45_f32, 1.0_f32,
            // second digit bbox -------------------------------------------
            0.55_f32, 0.0_f32, 0.8375_f32, 1.0_f32,
        ];
        let digits = vec![4, 2];

        (digits_bboxes, digits)
    } else {
        // IMPORTANT: by convention the "pinpad" is 10 digits, placed horizontally(side by side)
        // DO NOT change the layout, else wallet-app will NOT display the pinpad correctly!
        // That is b/c this layout in treated as a "texture atlas" so the positions MUST be known.
        // Ideally the positions SHOULD be passed from here all the way into the serialized .pgarbled/.packmsg
        // but this NOT YET the case.

        // 10 digits, 4 corners(vertices) per digit
        let mut digits_bboxes: Vec<f32> = Vec::with_capacity(10 * 4);
        /*
        for (int i = 0; i < 10; i++) {
            digits_bboxes.emplace_back(0.1f * i, 0.0f, 0.1f * (i + 1), 1.0f);
        }
        */
        for i in 0..10 {
            digits_bboxes.append(
                vec![
                    0.1_f32 * i as f32,
                    0.0_f32,
                    0.1_f32 * (i + 1) as f32,
                    1.0_f32,
                ]
                .as_mut(),
            );
        }
        let digits = vec![7, 8, 9, 0, 1, 2, 6, 5, 4, 3];

        (digits_bboxes, digits)
    };
    println!("using digits_bboxes: {digits_bboxes:?}");

    let circuit = generate_display_circuit(args.width, args.height, &digits_bboxes).unwrap();

    if let Some(output_path) = args.output_path {
        let buf = lib_circuits_rs::serialize(&circuit).unwrap();
        std::fs::write(output_path, &buf).unwrap();
    }

    let display_config = circuit.get_config().unwrap();
    let width = display_config.width as usize;
    let height = display_config.height as usize;

    let mut merged_outputs = vec![0u8; width * height];
    let mut temp_outputs = vec![];
    let mut rng = thread_rng();
    let rand_0_1 = Uniform::from(0..=1);

    let encoded_garbler_inputs =
        garbled_display_circuit_prepare_garbler_inputs(&circuit, &digits, &args.watermark_msg)
            .unwrap();

    let mut evaluator_inputs = prepare_evaluator_inputs(&circuit).unwrap();

    for _ in 0..args.nb_evals {
        // randomize the "rnd" part of the inputs
        // cf "rndswitch.v" comment above; DO NOT touch the last!
        for input in evaluator_inputs.iter_mut() {
            *input = rand_0_1.sample(&mut rng);
        }

        let inputs = [encoded_garbler_inputs.clone(), evaluator_inputs.clone()].concat();

        temp_outputs = circuit.eval_plain(&inputs).unwrap();
        assert_eq!(
            temp_outputs.len(),
            merged_outputs.len(),
            "outputs size mistmatch!"
        );

        for (merged_output, &cur_output) in merged_outputs.iter_mut().zip(temp_outputs.iter()) {
            // what we want is a OR:
            // 0 + 0 = 0
            // 1 + 0 = 1
            // 0 + 1 = 1
            // 1 + 1 = 1
            *merged_output = std::cmp::min(*merged_output + cur_output, 1u8)
        }
    }

    // convert (0,1) -> (0,255) to get a proper png
    for merged_output in merged_outputs.iter_mut() {
        *merged_output *= 255;
    }

    write_png("eval_outputs.png", width, height, &merged_outputs);
}
