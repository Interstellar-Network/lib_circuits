use serde::{Deserialize, Serialize};

#[cfg(feature = "alloc")]
use alloc::vec;
#[cfg(feature = "alloc")]
use alloc::vec::Vec;

#[derive(Debug, Clone, Copy, PartialEq, Serialize, Deserialize)]
#[repr(i32)]
pub enum GarblerInputsType {
    /// MUST be set to 0!
    Buf = 0,
    /// Part of 7 segments display; so 7 bits
    SevenSegments = 1,
    /// Part of the watermark; typically width*height nb pixels
    Watermark = 2,
}

#[derive(Debug, Clone, Copy, PartialEq, Serialize, Deserialize)]
#[repr(i32)]
pub enum EvaluatorInputsType {
    /// The "display circuit" standard input type: SHOULD be randomized during each eval loop
    Rnd = 0,
}

#[derive(Debug, Clone, Copy, PartialEq, Serialize, Deserialize)]
pub struct GarblerInputs {
    // TODO(lib-circuits-rs) NOT pub; and ideally the class itself
    pub r#type: GarblerInputsType,
    pub length: u32,
}

#[derive(Debug, Clone, Copy, PartialEq, Serialize, Deserialize)]
pub struct EvaluatorInputs {
    // TODO(lib-circuits-rs) NOT pub; and ideally the class itself
    pub r#type: EvaluatorInputsType,
    pub length: u32,
}

#[allow(clippy::module_name_repetitions)]
#[derive(Debug, Clone, PartialEq, Serialize, Deserialize, Default)]
pub struct DisplayConfig {
    pub garbler_inputs: Vec<GarblerInputs>,
    pub evaluator_inputs: Vec<EvaluatorInputs>,

    pub width: u32,
    pub height: u32,
}

impl DisplayConfig {
    /// This is where we add "display" semantics to what we write in Verilog define.v
    /// ie this is what allows us to transform a "generic circuit" into a "display circuit"
    ///
    /// cf src/segments2pixels/segments2pixels.h to see what parameter we need
    ///
    pub(crate) fn new(
        has_watermark: bool,
        width: u32,
        height: u32,
        nb_segments: u32,
        rndsize: u32,
    ) -> Self {
        let mut garbler_inputs = vec![
            GarblerInputs {
                r#type: GarblerInputsType::Buf,
                length: 1,
            },
            GarblerInputs {
                r#type: GarblerInputsType::SevenSegments,
                length: nb_segments,
            },
        ];
        if has_watermark {
            garbler_inputs.push(GarblerInputs {
                r#type: GarblerInputsType::Watermark,
                length: width * height,
            });
        }

        let evaluator_inputs = vec![EvaluatorInputs {
            r#type: EvaluatorInputsType::Rnd,
            length: rndsize,
        }];

        Self {
            garbler_inputs,
            evaluator_inputs,
            width,
            height,
        }
    }

    #[must_use]
    pub fn num_evaluator_inputs(&self) -> u32 {
        let mut num_inputs = 0;
        for skcd_input in &self.evaluator_inputs {
            num_inputs += skcd_input.length;
        }

        num_inputs
    }

    #[must_use]
    pub fn num_garbler_inputs(&self) -> u32 {
        let mut num_inputs = 0;
        for skcd_input in &self.garbler_inputs {
            num_inputs += skcd_input.length;
        }

        num_inputs
    }
}
