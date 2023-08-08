use serde::{Deserialize, Serialize};

#[cfg(feature = "alloc")]
use alloc::string::ToString;

use super::CircuitParserError;

/// This is a "reference" to either:
/// - another Gate's inputs
/// - a Gate's output
/// - a Circuit's output
// TODO ideally this SHOULD NOT be cloneable; and we should replace internal `id: usize` by eg `&Wire`
#[derive(Debug, Clone, PartialEq, Hash, Eq, Serialize, Deserialize)]
pub struct WireRef {
    pub id: usize,
}

#[allow(clippy::upper_case_acronyms)]
#[derive(Debug, Clone, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[repr(i32)]
pub enum KindBinary {
    // ZERO = 0,
    NOR = 1,
    // A-and-not-B
    // AANB = 2,
    // not-A-and-B?
    // NAAB = 4,
    XOR = 6,
    NAND = 7,
    AND = 8,
    XNOR = 9,
    // BUF = 10,
    // A-or-NOT-B?
    // AONB = 11,
    // BUFB = 12,
    // NOT-A-or-B?
    // NAOB = 13,
    OR = 14,
    // ONE = 15,
}

#[allow(clippy::upper_case_acronyms)]
#[derive(Debug, Clone, PartialEq, Eq, Hash, Serialize, Deserialize)]
#[repr(i32)]
pub enum KindUnary {
    // NOT B
    // INVB = 3,
    // NOT A
    INV = 5,
    BUF = 10,
}

// TODO use ?
// enum SkcdInput {
//     Garbler,
//     Evaluator,
//     /// Default: means the input is another gate's output
//     Default,
// }

/// For now in .skcd we have two kind of gates:
/// - standard eg: "8 = XOR(7,2)        // 8 = 7 xor Cin"
/// - constant eg: "3 = ZERO(0,0)" or "5 = ONE(0,0)"
/// Which means Constant type only has an output and NO input.
///
/// NOTE: it SHOULD be optimized-out by Verilog/ABC but right now, we CAN have multiple ZERO and ONE gates in a Circuit!
///
/// IMPORTANT: `gate_type` is only used for garling, it SHOULD NOT be sent to the evaluator!
/// For now, `GateType` is duplicated in repo `/lib-garble-rs/src/new_garbling_scheme/circuit_for_eval.rs`
/// with the field `gate_type` removed.
///
#[allow(clippy::module_name_repetitions)]
#[derive(Debug, PartialEq, Eq, Hash, Serialize, Deserialize, Clone)]
pub enum GateType {
    Binary {
        // TODO SHOULD be rewritten as "is_xor" to support Free XOR [when serializing]
        gate_type: KindBinary,
        input_a: WireRef,
        input_b: WireRef,
    },
    Unary {
        gate_type: KindUnary,
        input_a: WireRef,
    },
    /// Constant gates (ie 0 and 1) are a special case wrt to parsing the .skcd and garbling/evaluating:
    /// they are "rewritten" using AUX Gate (eg XOR(A,A) = 0, XNOR(A,A) = 1)
    /// That is because contrary to Unary gates, the paper does not explain how to
    /// generalize "Garbling other gate functionalities" to 0 input gate.
    Constant { value: bool },
}

#[derive(Debug, PartialEq, Eq, Hash, Serialize, Deserialize, Clone)]
pub struct Gate {
    pub(crate) internal: GateType,
    /// Gate's output is in practice a Gate's ID or idx
    pub(super) output: WireRef,
}

impl Gate {
    /// Called by `skcd_parser.rs`: build a new Gate based on a given `i32`
    /// which is a Protobuf `interstellarpbskcd::SkcdGateType`
    pub(crate) fn new_from_skcd_gate_type(
        blif_gate_type: &str,
        output: &WireRef,
        input_a: Option<&WireRef>,
        input_b: Option<&WireRef>,
    ) -> Result<Self, CircuitParserError> {
        let internal = match blif_gate_type {
            "XOR" => Ok(GateType::Binary {
                gate_type: KindBinary::XOR,
                input_a: input_a
                    .ok_or(CircuitParserError::BinaryGateMissingInputA)?
                    .clone(),
                input_b: input_b
                    .ok_or(CircuitParserError::BinaryGateMissingInputB)?
                    .clone(),
            }),
            "NAND" => Ok(GateType::Binary {
                gate_type: KindBinary::NAND,
                input_a: input_a
                    .ok_or(CircuitParserError::BinaryGateMissingInputA)?
                    .clone(),
                input_b: input_b
                    .ok_or(CircuitParserError::BinaryGateMissingInputB)?
                    .clone(),
            }),
            "NOR" => Ok(GateType::Binary {
                gate_type: KindBinary::NOR,
                input_a: input_a
                    .ok_or(CircuitParserError::BinaryGateMissingInputA)?
                    .clone(),
                input_b: input_b
                    .ok_or(CircuitParserError::BinaryGateMissingInputB)?
                    .clone(),
            }),
            "OR" => Ok(GateType::Binary {
                gate_type: KindBinary::OR,
                input_a: input_a
                    .ok_or(CircuitParserError::BinaryGateMissingInputA)?
                    .clone(),
                input_b: input_b
                    .ok_or(CircuitParserError::BinaryGateMissingInputB)?
                    .clone(),
            }),
            "AND" => Ok(GateType::Binary {
                gate_type: KindBinary::AND,
                input_a: input_a
                    .ok_or(CircuitParserError::BinaryGateMissingInputA)?
                    .clone(),
                input_b: input_b
                    .ok_or(CircuitParserError::BinaryGateMissingInputB)?
                    .clone(),
            }),
            "INV" => Ok(GateType::Unary {
                gate_type: KindUnary::INV,
                input_a: input_a
                    .ok_or(CircuitParserError::UnaryGateMissingInput)?
                    .clone(),
            }),
            "BUF" => Ok(GateType::Unary {
                gate_type: KindUnary::BUF,
                input_a: input_a
                    .ok_or(CircuitParserError::UnaryGateMissingInput)?
                    .clone(),
            }),
            "ZERO" => Ok(GateType::Constant { value: false }),
            "ONE" => Ok(GateType::Constant { value: true }),
            &_ => Err(CircuitParserError::UnknownGateType {
                gate_type: blif_gate_type.to_string(),
            }),
        }?;

        Ok(Self {
            internal,
            output: output.clone(),
        })
    }

    // TODO move to `impl Gate` directly; and remove `GateInternal`?
    #[must_use]
    pub fn get_type(&self) -> &GateType {
        &self.internal
    }

    #[must_use]
    pub fn get_id(&self) -> usize {
        self.get_output().id
    }

    #[must_use]
    pub fn get_output(&self) -> &WireRef {
        &self.output
    }
}
