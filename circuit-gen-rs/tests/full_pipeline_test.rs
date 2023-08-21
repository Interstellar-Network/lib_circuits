use lib_circuits_rs::{generate_display_circuit, new_from_verilog};

// all_inputs/all_expected_outputs: standard full-adder 2 bits truth table(and expected results)
// input  i_bit1;
// input  i_bit2;
// input  i_carry;
const FULL_ADDER_2BITS_ALL_INPUTS: [[u8; 3]; 8] = [
    [0, 0, 0],
    [1, 0, 0],
    [0, 1, 0],
    [1, 1, 0],
    [0, 0, 1],
    [1, 0, 1],
    [0, 1, 1],
    [1, 1, 1],
];

// output o_sum;
// output o_carry;
const FULL_ADDER_2BITS_ALL_EXPECTED_OUTPUTS: [[u8; 2]; 8] = [
    [0, 0],
    [1, 0],
    [1, 0],
    [0, 1],
    [1, 0],
    [0, 1],
    [0, 1],
    [1, 1],
];

/// Test the `pallet-ocw-circuits` use case
/// - generate a display circuit
/// - serialize it
///
#[test]
fn test_generate_display_message_120x52_2digits() {
    let digits_bboxes = vec![
        // first digit bbox --------------------------------------------
        0.25_f32, 0.1_f32, 0.45_f32, 0.9_f32,
        // second digit bbox -------------------------------------------
        0.55_f32, 0.1_f32, 0.75_f32, 0.9_f32,
    ];

    let circuit = generate_display_circuit(120, 52, &digits_bboxes).unwrap();

    let buf = circuit_types_rs::serialize(&circuit).unwrap();
    let ref_buf = include_bytes!("./data/result_display_message_120x52_2digits.postcard.bin");
    assert_eq!(buf, ref_buf);

    // TODO(lib-circuits-rs) aheck fields values, config, etc
    // assert_eq!(circuit., 5);
}

#[test]
fn test_new_from_verilog_adder_ok() {
    let circuit =
        new_from_verilog(include_bytes!("../../circuit-gen-rs/data/verilog/adder.v")).unwrap();

    let buf = circuit_types_rs::serialize(&circuit).unwrap();
    let ref_buf = include_bytes!("./data/result_abc_full_adder.postcard.bin");
    assert_eq!(buf, ref_buf);

    // TODO(lib-circuits-rs) aheck fields values, config, etc
    // assert_eq!(circuit., 5);
}

/// AUX function to generate a specific `LFSR_comb.v`
///
/// return: the outputs of the circuits
#[cfg(feature = "eval_plain")]
fn generic_lfsr_comb(nb_segments: usize, proba_verilog: &str) -> Vec<u8> {
    use rand::distributions::Distribution;
    use rand::distributions::Uniform;
    use rand::thread_rng;

    // For reference in C++:
    // auto mask_size = static_cast<unsigned int>(10);  // size of the LFSR_comb mask
    // rndsize_ = static_cast<unsigned int>(nb_segments_ * mask_size);
    const mask_size: usize = 10; // depends in the internals of circuit-gen-rs/data/verilog/LFSR_comb.v

    let rndsize = nb_segments * mask_size;

    let lfsr_buf = include_bytes!(concat!(
        env!("CARGO_MANIFEST_DIR"),
        "/data/verilog/LFSR_comb.v"
    ))
    .to_vec();

    let main_lfsr_buf = include_bytes!(concat!(
        env!("CARGO_MANIFEST_DIR"),
        "/tests/data/verilog/test-lfsr-comb.v"
    ))
    .to_vec();

    let define_buf = format!(
        r#"
            `define RNDSIZE {}
            `define BITMAP_NB_SEGMENTS {}
            `define PROBA {}
        "#,
        rndsize, nb_segments, proba_verilog
    )
    .as_bytes()
    .to_vec();
    let buf = [define_buf, lfsr_buf.clone(), main_lfsr_buf.clone()].concat();

    let circuit = new_from_verilog(&buf).unwrap();

    let mut inputs = vec![1; rndsize];

    let mut rng = thread_rng();
    let rand_0_1 = Uniform::from(0..=1);
    // randomize the "rnd" part of the inputs
    // cf "rndswitch.v" comment above; DO NOT touch the last!
    for input in inputs.iter_mut() {
        *input = rand_0_1.sample(&mut rng);
    }

    let outputs = circuit.eval_plain(&inputs).unwrap();

    outputs
}

/// Test only the sub circuit `circuit-gen-rs/data/verilog/LFSR_comb.v`
/// All probabilities SHOULD work correctly
///
/// NOTE; this one is quite slow! eg "finished in 145.52s" on local machine
#[cfg(feature = "eval_plain")]
#[test]
fn test_generic_lfsr_comb_all_probas() {
    const nb_segments: usize = 1000; // pick anything big enough to have good probabilities (Law of large numbers)

    // NOTE: the probas are not exactly spot-on; so the range is sometimes shifted a little bit, and/or quite big
    for ((proba_verilog, proba_human_readable), (range_low, range_high)) in vec![
        (("4'b0110", 0.5), (0.45, 0.59)),
        (("4'b0101", 0.6), (0.55, 0.69)),
        (("4'b0100", 0.7), (0.65, 0.79)),
        (("4'b0011", 0.8), (0.75, 0.89)),
        (("4'b0010", 0.9), (0.85, 0.99)),
        (("4'b0001", 0.99), (0.95, 0.999)),
        (("4'b0000", 1.0), (0.99, 1.0)),
    ] {
        let outputs = generic_lfsr_comb(nb_segments, proba_verilog);

        assert!(
            outputs.iter().all(|&n| n <= 1),
            "outputs contain something else than 0 or 1!"
        );

        let ones_relative =
            outputs.iter().filter(|&n| *n == 1).count() as f64 / outputs.len() as f64;
        println!(
            "proba: {proba_verilog} {proba_human_readable}, ones_relative: {:#?}",
            ones_relative,
        );
        assert!(
            ones_relative >= range_low && ones_relative <= range_high,
            "proba outside of range!"
        );
    }
}

/// Test only the sub circuit `circuit-gen-rs/data/verilog/LFSR_comb.v`
/// The outputs SHOULD be random(ie no output[i] should be "stuck" to a given value)
#[cfg(feature = "eval_plain")]
#[test]
fn test_generic_lfsr_comb_outputs_changing() {
    const nb_segments: usize = 10;

    let (proba_verilog, proba_human_readable) = ("4'b0110", 0.5);

    let outputs_0 = generic_lfsr_comb(nb_segments, proba_verilog);
    let mut outputs_changed = vec![false; outputs_0.len()];

    for i in 0..10 {
        let outputs = generic_lfsr_comb(nb_segments, proba_verilog);

        for (idx, o) in outputs.iter().enumerate() {
            // if the output has NOT YET changed (== switch to true at some point)
            // AND it is different than the first round (_0 round)
            // -> mark it has changed
            if (!outputs_changed[idx]) && (*o != outputs_0[idx]) {
                outputs_changed[idx] = true;
            }
        }

        println!("outputs_changed[{i}]: {outputs_changed:#?}",);
    }

    println!(" outputs_changed: {outputs_changed:#?}",);
    assert!(
        outputs_changed.iter().all(|&n| n),
        "outputs SHOULD have all changed in 10+ rounds!"
    );
}

#[cfg(feature = "eval_plain")]
#[test]
fn test_new_from_verilog_adder_eval_plain_ok() {
    let circuit =
        new_from_verilog(include_bytes!("../../circuit-gen-rs/data/verilog/adder.v")).unwrap();

    for (i, inputs) in FULL_ADDER_2BITS_ALL_INPUTS.iter().enumerate() {
        let outputs = circuit.eval_plain(inputs).unwrap();
        assert_eq!(outputs, FULL_ADDER_2BITS_ALL_EXPECTED_OUTPUTS[i]);
    }
}

/// [generic circuit] An invalid Verilog should fail gracefully
/// And more generally, all the C++ except/errors/etc SHOULD be caught!
/// This is really important when called from the pallets(Substrate and Integritee)
#[test]
fn test_new_from_verilog_bad_file_fail_gracefully() {
    let bad_data = vec![42, 42];
    let circuit = new_from_verilog(&bad_data);

    assert!(circuit.is_err());
}
