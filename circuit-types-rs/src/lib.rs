// #![no_std]
// https://github.com/substrate-developer-hub/substrate-module-template/blob/master/HOWTO.md#forgetting-cfg_attr-for-no_std
#![cfg_attr(not(feature = "std"), no_std)]
#![deny(elided_lifetimes_in_paths)]
#![warn(clippy::suspicious)]
#![warn(clippy::complexity)]
#![warn(clippy::perf)]
#![warn(clippy::style)]
#![warn(clippy::pedantic)]
#![warn(clippy::expect_used)]
#![warn(clippy::panic)]
#![warn(clippy::unwrap_used)]

#[cfg(feature = "alloc")]
extern crate alloc;

#[cfg(feature = "std")]
#[macro_use]
extern crate std;

#[cfg(feature = "blif_parser")]
mod blif_parser;
mod config;
mod gate;
mod gate_id_to_wire_ref;
mod metadata;
mod serialize_deserialize;

use serde::{Deserialize, Serialize};
use snafu::Snafu;

#[cfg(feature = "alloc")]
use alloc::string::String;
#[cfg(feature = "alloc")]
use alloc::vec;
#[cfg(feature = "alloc")]
use alloc::vec::Vec;

pub use config::{DisplayConfig, EvaluatorInputsType, GarblerInputsType};
pub use gate::{Gate, GateType, KindBinary, KindUnary, WireRef};
pub use metadata::Metadata;

use serialize_deserialize::{deserialize_from_buffer_postcard, serialize_to_buffer_postcard};

/// A basic boolean circuit
/// It SHOULD roughly match `struct Blif`; but with the `BlifIdentifier` convert from str -> integer
///
/// "Circuit syntax. A Boolean circuit C : {0, 1}n → {0, 1}m has n input wires
/// enumerated by the indices 1, . . . , n, and m output wires enumerated by n + q −
/// m + 1, . . . , n + q, where q = |C| is the number Boolean gates. The output wire
/// of gate j (also denoted by gj ) is n + j,"
///
/// NOTE: this is important, especially for the outputs to be in order!
/// ie DO NOT use HashSet/HashMap etc in this struct!
#[derive(Serialize, Deserialize, PartialEq, Debug, Clone)]
pub(crate) struct CircuitInternal {
    pub(crate) inputs: Vec<WireRef>,
    pub(crate) outputs: Vec<WireRef>,
    /// Transform Vec<Gates> -> Vec<Vec<Gates>>
    /// That is how we represent the "layers"; ie what will be garbleable / evaluable in parallel
    /// eg:
    /// - gate layer 0: gates with NO inputs dependency
    /// - gate layer 1: gates whose at least one input depends on the "layer 0" outputs
    /// - etc
    pub(crate) gates: Vec<Vec<gate::Gate>>,
    pub(crate) wires: Vec<WireRef>,
}

/// Serialize a Circuit
///
/// Step 2: called from pallet-ocw-circuits, using the `Circuit` from Step 1
///     The buffer is then stored in IPFS
///
/// # Errors
///
/// - `CircuitParserError::SerializerDeserializerPostcardError` if Postcard `to_allocvec` failed.
///    Possibly only happen in case of bad alloc?
///
pub fn serialize(circuit: &Circuit) -> Result<Vec<u8>, CircuitParserError> {
    serialize_to_buffer_postcard(circuit)
}

/// Deserialize Circuit
///
/// Step 3: called from pallet-ocw-garble, using the `Circuit`
///     Using the data retrieve from IPFS
///
/// # Errors
///
/// - `CircuitParserError::SerializerDeserializerPostcardError` if the given buffer is not a valid
///   Postcard-serialized Circuit
///
pub fn deserialize_from_buffer(buf: &[u8]) -> Result<Circuit, CircuitParserError> {
    deserialize_from_buffer_postcard(buf)
}

/// Represents a raw(ie **UN**garbled) circuit; usually created from a `.blif.blif` file(previously `.skcd`)
/// and its associated "config":
/// - `CircuitMetadata`: is a mandatory field used later when garling; it is just a list of min/max,indexes,etc
/// - `CircuitDisplayConfig`: is only set for "display circuits" eg "full adder" and other "generic circuits" DO NOT need it
///   It is used both when garling to add the semantic "garbler inputs" vs "evaluator inputs" to the Circuit,
///   and in a simplified form (= most fields removed) when evaluating (eg to set the proper texture size we need width and height)
///
#[derive(Debug, PartialEq, Serialize, Deserialize, Clone)]
pub struct Circuit {
    circuit: CircuitInternal,
    metadata: Metadata,
    /// Set only for "display circuits"
    display_config: Option<DisplayConfig>,
}

/// Errors emitted by the circuit parser.
#[derive(Debug, Snafu)]
pub enum CircuitParserError {
    /// Error at Circuit::new_*: the buffer MUST contain valid UTF8
    /// It SHOULD be the data from ".blif.blif" file
    InvalidBufferNotUTF8,
    /// Error at `blif_parser::parse_blif`
    BlifParserError {
        msg: String,
    },
    /// InvalidGateIdError: the given GateID from the .skcd DOES NOT match CircuitBuilder's
    InvalidGateId {
        gate_id: String,
    },
    /// when building the `outputs`, the GateID MUST have be created earlier
    /// This is a "sub-enum" of InvalidGateId
    OutputInvalidGateId {
        gate_id: String,
    },
    UnknownGateType {
        gate_type: String,
    },
    UnaryGateMissingInput,
    BinaryGateMissingInputA,
    BinaryGateMissingInputB,
    SerializerDeserializerPostcardError {
        err: postcard::Error,
    },
    /// FFI: exception thrown from C++
    CppException {
        msg: String,
    },
}

/// Errors emitted when using `Circuit`
#[derive(Debug, Snafu)]
pub enum CircuitError {
    /// Try to call `get_config` on a "generic circuit"
    /// The `display_config` is Optional, and only set for "display circuits"
    MissingConfigNotADisplayCircuit,
}

impl Circuit {
    #[must_use]
    pub fn get_inputs(&self) -> &[WireRef] {
        &self.circuit.inputs
    }

    #[must_use]
    pub fn get_outputs(&self) -> &[WireRef] {
        &self.circuit.outputs
    }

    #[must_use]
    pub fn get_config(&self) -> Option<&DisplayConfig> {
        self.display_config.as_ref()
    }

    #[must_use]
    pub fn get_metadata(&self) -> &Metadata {
        &self.metadata
    }

    #[must_use]
    pub fn get_nb_wires(&self) -> usize {
        self.circuit.wires.len()
    }

    #[must_use]
    pub fn get_nb_inputs(&self) -> usize {
        self.circuit.inputs.len()
    }

    #[must_use]
    pub fn get_nb_outputs(&self) -> usize {
        self.circuit.outputs.len()
    }

    #[must_use]
    pub fn get_gates(&self) -> &Vec<Vec<Gate>> {
        &self.circuit.gates
    }

    #[must_use]
    pub fn get_wires(&self) -> &Vec<WireRef> {
        &self.circuit.wires
    }

    /// TEST ONLY; called from repo `lib-garble-rs`
    /// Build a basic circuit containing only a desired Binary Gate
    #[doc(hidden)]
    #[must_use]
    pub fn new_test_circuit(gate_binary_type: gate::KindBinary) -> Self {
        Self {
            circuit: CircuitInternal {
                inputs: vec![WireRef { id: 0 }, WireRef { id: 1 }],
                outputs: vec![WireRef { id: 2 }],
                gates: vec![vec![Gate {
                    internal: GateType::Binary {
                        gate_type: gate_binary_type,
                        input_a: WireRef { id: 0 },
                        input_b: WireRef { id: 1 },
                    },
                    output: WireRef { id: 2 },
                }]],
                wires: vec![WireRef { id: 0 }, WireRef { id: 1 }, WireRef { id: 2 }],
            },
            display_config: None,
            metadata: Metadata::new_for_test((2, 2), 2),
        }
    }

    /// TEST ONLY; called from repo `lib-garble-rs`
    #[doc(hidden)]
    #[must_use]
    pub fn new_test_circuit_unary(gate_unary_type: gate::KindUnary) -> Self {
        Self {
            circuit: CircuitInternal {
                inputs: vec![WireRef { id: 0 }],
                outputs: vec![WireRef { id: 1 }],
                gates: vec![vec![Gate {
                    internal: GateType::Unary {
                        gate_type: gate_unary_type,
                        input_a: WireRef { id: 0 },
                    },
                    output: WireRef { id: 1 },
                }]],
                wires: vec![WireRef { id: 0 }, WireRef { id: 1 }],
            },
            display_config: None,
            metadata: Metadata::new_for_test((1, 1), 1),
        }
    }

    /// TEST ONLY; called from repo `lib-garble-rs`
    #[doc(hidden)]
    #[must_use]
    pub fn new_test_circuit_constant(value: bool) -> Self {
        Self {
            circuit: CircuitInternal {
                inputs: vec![WireRef { id: 0 }],
                outputs: vec![WireRef { id: 1 }],
                gates: vec![vec![Gate {
                    internal: GateType::Constant { value },
                    output: WireRef { id: 1 },
                }]],
                wires: vec![WireRef { id: 0 }, WireRef { id: 1 }],
            },
            display_config: None,
            metadata: Metadata::new_for_test((1, 1), 1),
        }
    }
}

#[cfg(any(test, feature = "eval_plain"))]
#[derive(Debug, Snafu)]
pub enum EvaluateError {
    Unknown,
}

#[cfg(any(test, feature = "eval_plain"))]
impl Circuit {
    /// Evaluate (clear text version == UNGARBLED) using crate "`boolean_expression`"
    /// NOTE: here there is no concept of "`evaluator_inputs`" vs "`garbler_inputs`" b/c this is only
    /// used by tests/examples.
    ///
    /// See also: https://stackoverflow.com/questions/59109453/how-do-i-use-the-rust-crate-boolean-expression-to-implement-a-simple-logic-cir
    ///
    /// * param inputs: treated as booleans: 0 -> false, >=1 = true
    ///
    pub fn eval_plain(&self, inputs: &[u8]) -> Result<Vec<u8>, EvaluateError> {
        use boolean_expression::{BDDFunc, BDD_ONE, BDD_ZERO};

        // TODO(lib-circuits-rs) re-add num_evaluator_inputs etc
        // assert!(
        //     self.num_evaluator_inputs() == self.circuit.n(),
        //     "only basic circuits without garbler inputs! [1]"
        // );
        // assert!(
        //     self.num_garbler_inputs() == 0,
        //     "only basic circuits without garbler inputs! [2]"
        // );
        assert_eq!(
            inputs.len(),
            self.get_nb_inputs(),
            "missing inputs: wrong length!"
        );

        let mut circuit = boolean_expression::BDD::new();
        // Map: "WireRef" == Gate ID to a BDDFunc
        let mut bdd_map = hashbrown::HashMap::new();

        for (idx, input_wire) in self.circuit.inputs.iter().enumerate() {
            // cf `hashmap_inputs` below to see why we CHECK
            assert_eq!(idx, input_wire.id, "input Wire not in order!");

            bdd_map.insert(input_wire.id, circuit.terminal(input_wire.id));
        }

        // cf https://github.com/trailofbits/mcircuit/blob/8fe9b315f2e8cae6020a2884ae544d59bd0bbd41/src/parsers/blif.rs#L194
        // For how to match blif/skcd gates into mcircuit's Operation
        // WARNING: apparently Operation::XXX is (OUTPUT, INPUT1, etc)! OUTPUT IS FIRST!
        for gate in self.circuit.gates.iter().flatten() {
            let bdd_gate: BDDFunc = match gate.get_type() {
                GateType::Binary {
                    gate_type: r#type,
                    input_a,
                    input_b,
                } => match r#type {
                    KindBinary::XOR => circuit.xor(
                        *bdd_map
                            .get(&input_a.id)
                            .expect("GateType::XOR missing input a!"),
                        *bdd_map
                            .get(&input_b.id)
                            .expect("GateType::XOR missing input b!"),
                    ),
                    KindBinary::XNOR => {
                        // XNOR is a XOR, whose output is NOTed
                        let xor_output = circuit.xor(
                            *bdd_map
                                .get(&input_a.id)
                                .expect("GateType::XOR missing input a!"),
                            *bdd_map
                                .get(&input_b.id)
                                .expect("GateType::XOR missing input b!"),
                        );

                        circuit.not(xor_output)
                    }
                    KindBinary::NAND => {
                        // NAND is a AND, whose output is NOTed
                        let and_output = circuit.and(
                            *bdd_map
                                .get(&input_a.id)
                                .expect("GateType::NAND missing input a!"),
                            *bdd_map
                                .get(&input_b.id)
                                .expect("GateType::NAND missing input b!"),
                        );

                        circuit.not(and_output)
                    }
                    KindBinary::NOR => {
                        // NOR is a OR, whose output is NOTed
                        let or_output = circuit.or(
                            *bdd_map
                                .get(&input_a.id)
                                .expect("GateType::NOR missing input a!"),
                            *bdd_map
                                .get(&input_b.id)
                                .expect("GateType::NOR missing input b!"),
                        );

                        circuit.not(or_output)
                    }
                    KindBinary::AND => circuit.and(
                        *bdd_map
                            .get(&input_a.id)
                            .expect("GateType::AND missing input a!"),
                        *bdd_map
                            .get(&input_b.id)
                            .expect("GateType::AND missing input b!"),
                    ),
                    KindBinary::OR => circuit.or(
                        *bdd_map
                            .get(&input_a.id)
                            .expect("GateType::OR missing input a!"),
                        *bdd_map
                            .get(&input_b.id)
                            .expect("GateType::OR missing input b!"),
                    ),
                },
                GateType::Unary {
                    gate_type: r#type,
                    input_a,
                } => match r#type {
                    KindUnary::INV => circuit.not(
                        *bdd_map
                            .get(&input_a.id)
                            .expect("GateType::NOT missing input a!"),
                    ),
                    // ite = If-Then-Else
                    // we define BUF as "if input == 1 then input; else 0"
                    KindUnary::BUF => circuit.ite(
                        *bdd_map
                            .get(&input_a.id)
                            .expect("GateType::NOT missing input a!"),
                        BDD_ONE,
                        BDD_ZERO,
                    ),
                },
                GateType::Constant { value } => circuit.constant(*value),
            };

            bdd_map.insert(gate.get_id(), bdd_gate);
        }

        ////////////////////////////////////////////////////////////////////////

        let circuit = circuit.clone();

        // cf boolean_expression examples/tests for how the evaluation works
        // https://github.com/cfallin/boolean_expression/blob/795b89567e05f54907b89453bdd481d0b01f0c93/src/bdd.rs#L1071
        let hashmap_inputs = inputs
            .iter()
            .enumerate()
            .map(|(idx, input)| (idx, *input >= 1))
            .collect();

        // TODO? this only works when output[Y] = fn(input[X])
        // ie when there is only one layer!
        // -> FAIL for "display circuits"?
        //
        let res_outputs: Vec<u8> = self
            .circuit
            .outputs
            .iter()
            .map(|output| {
                let output_bddfunc = *bdd_map.get(&output.id).expect("missing output!");
                u8::from(circuit.evaluate(output_bddfunc, &hashmap_inputs))
            })
            .collect();
        //
        // FAIL! not needed anyway, above works fine...
        //
        // let mut res_outputs: Vec<u8> = vec![];
        // for gate in &self.circuit.gates {
        //     let bdd_gate = bdd_map.get(&gate.get_id()).expect("missing gate!");

        //     let res = circuit.evaluate(*bdd_gate, &hashmap_inputs);

        //     hashmap_inputs.insert(gate.get_id(), res);

        //     if self.get_metadata().gate_idx_is_output(gate.get_id()) {
        //         res_outputs.push(u8::from(res));
        //     }
        // }

        Ok(res_outputs)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    // all_inputs/all_expected_outputs: standard full-adder 2 bits truth table(and expected results)
    // input  i_bit1;
    // input  i_bit2;
    // input  i_carry;
    pub(super) const FULL_ADDER_2BITS_ALL_INPUTS: [[u8; 3]; 8] = [
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
    pub(super) const FULL_ADDER_2BITS_ALL_EXPECTED_OUTPUTS: [[u8; 2]; 8] = [
        [0, 0],
        [1, 0],
        [1, 0],
        [0, 1],
        [1, 0],
        [0, 1],
        [0, 1],
        [1, 1],
    ];

    #[test]
    fn test_deserialize_and_eval_full_adder_2bits() {
        let circ = deserialize_from_buffer(include_bytes!(
            "../../circuit-gen-rs/tests/data/result_abc_full_adder.postcard.bin"
        ))
        .unwrap();

        for (i, inputs) in FULL_ADDER_2BITS_ALL_INPUTS.iter().enumerate() {
            let outputs = circ.eval_plain(inputs).unwrap();
            assert_eq!(outputs, FULL_ADDER_2BITS_ALL_EXPECTED_OUTPUTS[i]);
        }
    }

    #[cfg(feature = "blif_parser")]
    #[test]
    fn test_eval_plain_full_adder_2bits() {
        let circ = Circuit::new_generic_from_blif_buffer(include_bytes!(
            "../../circuit-gen-rs/tests/data/result_abc_full_adder.blif.blif"
        ))
        .unwrap();

        // TODO(lib-circuits-rs) re-add num_evaluator_inputs etc
        // assert!(circ.num_evaluator_inputs() == 3);
        for (i, inputs) in FULL_ADDER_2BITS_ALL_INPUTS.iter().enumerate() {
            let outputs = circ.eval_plain(inputs).unwrap();
            assert_eq!(outputs, FULL_ADDER_2BITS_ALL_EXPECTED_OUTPUTS[i]);
        }
    }

    #[cfg(feature = "blif_parser")]
    #[test]
    fn test_eval_plain_display_message_120x52_2digits() {
        let circ = Circuit::new_display_from_blif_buffer(
            include_bytes!(
                "../../circuit-gen-rs/tests/data/result_display_message_120x52_2digits.blif.blif"
            ),
            true,
            120,
            52,
            4242,
            4242,
        )
        .unwrap();

        let inputs = vec![0; 6395];
        let outputs = circ.eval_plain(&inputs).unwrap();
        // TODO compare with reference .png? But that would require multiple eval which is quite slow
        assert_eq!(outputs.len(), 120 * 52);
    }
}
