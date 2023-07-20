/// Implement de/serialization using Postcard <https://github.com/jamesmunns/postcard>
/// Why not others?
/// - msgpack rust: NOT compatible with `no_std`(and therefore fail in SGX env)
///   "rmp" crate SHOULD work, but "rmp-serde" definitely DOES NOT...
/// - prost: COULD works OK but we must re-implement all (de)serialization manually instead
///   of being able to re-use the Swanky provided "serde1" feature.
///   WOULD also require to add a few getters to expose deltas/Block/etc
///   NOTE: works in `no_std/sgx` only when using pregenerated .rs
///
// TODO see others https://serde.rs/#data-formats
use postcard::{from_bytes, to_allocvec};

use crate::{Circuit, CircuitParserError};

#[cfg(feature = "alloc")]
use alloc::vec::Vec;

/// Serialize
/// Our use case only requires a subset of the whole (de)serialization so no need to expose the whole module
///# Errors
///
/// `postcard::Error` if the serialization failed
///
pub fn serialize_to_buffer_postcard(circuit: &Circuit) -> Result<Vec<u8>, CircuitParserError> {
    let buf: Vec<u8> = to_allocvec(circuit)
        .map_err(|err| CircuitParserError::SerializerDeserializerPostcardError { err })?;

    Ok(buf)
}

/// Deserialize
/// Our use case only requires a subset of the whole (de)serialization so no need to expose the whole module
///
/// # Errors
///
/// `postcard::Error` if the deserialization failed
///
pub fn deserialize_from_buffer_postcard(buf: &[u8]) -> Result<Circuit, CircuitParserError> {
    let circuit: Circuit = from_bytes(buf)
        .map_err(|err| CircuitParserError::SerializerDeserializerPostcardError { err })?;

    Ok(circuit)
}

#[cfg(test)]
mod tests {
    use super::*;

    /// test that specific(=postcard) (de)serialization works
    ///
    /// Make sure that deserialize_from_buffer_postcard(serialize_to_buffer_postcard(a)) == a
    #[cfg(feature = "blif_parser")]
    #[test]
    fn test_serialize_deserialize_full_adder_2bits() {
        let ref_circuit = Circuit::new_generic_from_blif_buffer(include_bytes!(
            "../../circuit-gen-rs/tests/data/result_abc_full_adder.blif.blif"
        ))
        .unwrap();

        let buf = serialize_to_buffer_postcard(&ref_circuit).unwrap();
        let new_circuit = deserialize_from_buffer_postcard(&buf).unwrap();

        assert_eq!(ref_circuit, new_circuit);
    }

    /// Compare `serialize_to_buffer_postcard` with a golden reference
    /// This ensures that the file format DOES NOT change
    #[cfg(feature = "blif_parser")]
    #[test]
    fn test_serialize_golden_full_adder_2bits() {
        let ref_circuit = Circuit::new_generic_from_blif_buffer(include_bytes!(
            "../../circuit-gen-rs/tests/data/result_abc_full_adder.blif.blif"
        ))
        .unwrap();

        let buf = serialize_to_buffer_postcard(&ref_circuit).unwrap();
        let ref_buf =
            include_bytes!("../../circuit-gen-rs/tests/data/result_abc_full_adder.postcard.bin");

        assert_eq!(buf, ref_buf);
    }

    #[cfg(feature = "blif_parser")]
    #[test]
    fn test_serialize_golden_display_message_120x52_2digits() {
        let ref_circuit = Circuit::new_display_from_blif_buffer(
            include_bytes!(
                "../../circuit-gen-rs/tests/data/result_display_message_120x52_2digits.blif.blif"
            ),
            true,
            120,
            52,
            2 * 7,
            2 * 7 * 10,
        )
        .unwrap();

        let buf = serialize_to_buffer_postcard(&ref_circuit).unwrap();
        let ref_buf = include_bytes!(
            "../../circuit-gen-rs/tests/data/result_display_message_120x52_2digits.postcard.bin"
        );

        assert_eq!(buf, ref_buf);
    }

    // /// test that specific(=postcard) (de)serialization works with `display_message_120x52_2digits`
    // /// NOTE: contrary to "generic circuits"(cf above) we HAVE set some "`garbler_inputs`" in the Encoder and those SHOULD NOT
    // /// be serialized(cf test after) so we compare manually
    // #[test]
    // fn test_serialize_deserialize_display_message_120x52_2digits() {
    //     let mut ref_circuit = garble_skcd(include_bytes!(
    //         "../examples/data/display_message_120x52_2digits.skcd.pb.bin"
    //     ))
    //     .unwrap();
    //     let garbler_inputs = vec![0; ref_circuit.num_garbler_inputs() as usize];
    //     let encoded_garbler_inputs = ref_circuit.encode_garbler_inputs(&garbler_inputs);

    //     let buf = serialize_to_buffer_postcard(ref_circuit.clone(), encoded_garbler_inputs).unwrap();
    //     let (new_garb, _new_encoded_garbler_inputs) =
    //         deserialize_from_buffer_postcard(&buf).unwrap();

    //     garbled_circuit_reset_gate_type(&mut ref_circuit);

    //     assert_eq!(ref_circuit, new_garb);
    //     assert_eq!(
    //         ref_circuit.num_evaluator_inputs(),
    //         new_garb.num_evaluator_inputs()
    //     );
    //     assert_eq!(ref_circuit.config, new_garb.config);
    // }

    // #[test]
    // fn test_serialize_golden_display_message_120x52_2digits() {
    //     let ref_circuit = garble_skcd_with_seed(
    //         include_bytes!("../examples/data/display_message_120x52_2digits.skcd.pb.bin"),
    //         424242,
    //     )
    //     .unwrap();
    //     let garbler_inputs = vec![4, 2];
    //     let encoded_garbler_inputs = garbled_display_circuit_prepare_garbler_inputs(
    //         &ref_garb,
    //         &garbler_inputs,
    //         "test message",
    //     )
    //     .unwrap();

    //     let buf = serialize_to_buffer_postcard(ref_garb, encoded_garbler_inputs).unwrap();

    //     let ref_buf =
    //         include_bytes!("../examples/data/display_message_120x52_2digits.garbled.pb.bin");

    //     assert_eq!(buf, ref_buf, "failed {buf:#?} vs {ref_buf:#?}");
    // }

    // /// test that the client DOES NOT have access to Encoder's `garbler_inputs`
    // #[test]
    // // TODO(security) [security] we SHOULD NOT be able to call `encoding_internal` after `(de)serialize_for_evaluator`
    // //  cf `InputEncodingSet` -> SHOULD probably be refactored(splitted) into "garbler" vs "evaluator"
    // #[ignore]
    // fn test_encoder_has_no_garbler_inputs_display_message_120x52_2digits() {
    //     let mut ref_garb = garble_skcd(include_bytes!(
    //         "../examples/data/display_message_120x52_2digits.skcd.pb.bin"
    //     ))
    //     .unwrap();
    //     let garbler_inputs = vec![0; ref_garb.num_garbler_inputs() as usize];
    //     let encoded_garbler_inputs = ref_garb.encode_garbler_inputs(&garbler_inputs);

    //     let buf = serialize_to_buffer_postcard(ref_garb.clone(), encoded_garbler_inputs).unwrap();
    //     let (new_garb, _new_encoded_garbler_inputs) =
    //         deserialize_from_buffer_postcard(&buf).unwrap();

    //     garbled_circuit_reset_gate_type(&mut ref_garb);

    //     assert_eq!(new_garb.num_garbler_inputs(), 0);
    // }

    // /// IMPORTANT: for security/privacy, we DO NOT serialize the `GateType`
    // /// so we must clean the Gate
    // fn garbled_circuit_reset_gate_type(garbled: &mut GarbledCircuit) {
    //     for gate in &mut garbled.garbled.circuit.gates {
    //         match &mut gate.internal {
    //             crate::circuit::GateType::Binary {
    //                 ref mut gate_type,
    //                 input_a: _,
    //                 input_b: _,
    //             } => *gate_type = None,
    //             crate::circuit::GateType::Unary {
    //                 ref mut gate_type,
    //                 input_a: _,
    //             } => *gate_type = None,
    //             crate::circuit::GateType::Constant { value: _ } => {}
    //         }
    //     }
    // }
}
