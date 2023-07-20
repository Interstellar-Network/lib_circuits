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
