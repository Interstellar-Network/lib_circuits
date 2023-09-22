//! TODO maybe see <https://github.com/trailofbits/mcircuit/blob/main/src/parsers/blif.rs>
//! and <https://github.com/rust-bakery/nom/blob/main/doc/nom_recipes.md#implementing-fromstr>

use hashbrown::HashMap;
use nom::{
    branch::alt,
    bytes::complete::{is_not, tag, take_while1},
    character::{
        complete::{alpha1, alphanumeric1, char, line_ending, multispace1, space1},
        is_alphanumeric,
    },
    combinator::{opt, recognize, value},
    error::ParseError,
    multi::{many0, many0_count, many1},
    sequence::{pair, preceded},
    IResult, Parser,
};

#[cfg(feature = "alloc")]
use alloc::string::String;
#[cfg(feature = "alloc")]
use alloc::string::ToString;
#[cfg(feature = "alloc")]
use alloc::vec;
#[cfg(feature = "alloc")]
use alloc::vec::Vec;

extern crate alloc;

use crate::{
    gate_id_to_wire_ref::SkcdToWireRefConverter, metadata::Metadata, Circuit, CircuitInternal,
    CircuitParserError, DisplayConfig, Gate,
};

// https://github.com/rust-bakery/nom/blob/main/doc/custom_input_types.md#implementing-a-custom-type
type MyInput<'a> = &'a str;
type Output<'a> = &'a str;
// type MyInput<'a> = &[u8];
// type Output<'a> = &[u8];

#[derive(PartialEq, Debug)]
pub(crate) struct BlifIdentifier {
    pub(crate) id: String,
}

/// This is "basic copy" of the C++ struct `BlifParser`
#[derive(PartialEq, Debug)]
pub(crate) struct Blif {
    pub(crate) gates: Vec<BlifGate>,
    pub(crate) inputs: Vec<BlifIdentifier>,
    pub(crate) outputs: Vec<BlifIdentifier>,
}

#[derive(PartialEq, Debug)]
pub(crate) struct BlifGate {
    pub(crate) gate_type: String,

    // a and/or b can be null
    // o is always set
    pub(crate) a: Option<BlifIdentifier>,
    pub(crate) b: Option<BlifIdentifier>,
    pub(crate) o: BlifIdentifier,
}

impl From<&str> for BlifIdentifier {
    fn from(value: &str) -> Self {
        Self {
            id: value.to_string(),
        }
    }
}

/// Return if a given char is a valid Blif identifier
/// ie can it be used for model/input/output/etc
fn is_valid_char_id(chr: char) -> bool {
    is_alphanumeric(chr as u8) || chr == '-' || chr == '_'
}

/// Return if a given char is a valid Blif identifier
/// ie can it be used for model/input/output/etc
///
/// <https://github.com/rust-bakery/nom/blob/main/doc/nom_recipes.md#identifiers>
fn blif_identifier_internal(input: MyInput<'_>) -> IResult<MyInput<'_>, Output<'_>> {
    recognize(pair(
        alt((alpha1, tag("_"), tag("-"), tag("["), tag("]"))),
        many0_count(alt((alphanumeric1, tag("_"), tag("-"), tag("["), tag("]")))),
    ))(input)
}

/// Eat the leading whitespace/newlines/tab
///
/// "A combinator that takes a parser `inner` and produces a parser that also consumes both leading and
/// trailing whitespace, returning the output of `inner`."
///
/// <https://github.com/rust-bakery/nom/blob/main/doc/nom_recipes.md#wrapper-combinators-that-eat-whitespace-before-and-after-a-parser>
fn leading_ws<'a, F, O, E: ParseError<&'a str>>(
    inner: F,
) -> impl FnMut(&'a str) -> IResult<&'a str, O, E>
where
    F: Parser<&'a str, O, E>,
{
    preceded(many0(alt((multispace1, tag("\\")))), inner)
}

///
/// <https://www.reddit.com/r/rust/comments/11lh439/nom_parsing_multiline_blocks_separated_by_empty/>
fn blif_identifier(input: MyInput<'_>) -> IResult<MyInput<'_>, Output<'_>> {
    // preceded(multispace1, blif_identifier_internal)(input)
    let (i, ident) = leading_ws(blif_identifier_internal)(input)?;

    Ok((i, ident))
}

/// Parse a block of Blif identifier
/// It can be:
/// - on one line, with a \n: eg ".inputs a b cin\n" -> ["a", "b", "cin"]
/// - or split on multiple lines ".inputs a b c \\n d e f\n" -> ["a", "b", "c" , "d" ,"e", "f"]
///   NOTE: the lines are delimited by '\' AND a new line '\n'
///
/// In both cases, the final '\n' is mandatory.
///
fn blif_identifiers_list(input: MyInput<'_>) -> IResult<MyInput<'_>, Vec<Output<'_>>> {
    let (i, idents) = many1(blif_identifier)(input)?;

    let (i, _) = line_ending(i)?;

    Ok((i, idents))
}

/// Parse the model line: eg `.model full_add\n`
fn parse_model(i: MyInput<'_>) -> IResult<MyInput<'_>, Output<'_>> {
    let (i, _) = opt(hashtag_comment)(i)?;

    let (i, _) = tag(".model")(i)?;
    let (i, _) = space1(i)?;
    let (i, model) = take_while1(is_valid_char_id)(i)?;
    let (i, _) = line_ending(i)?;

    Ok((i, model))
}

/// Parse the model line: eg ".inputs a b cin\n"
fn parse_inputs(i: MyInput<'_>) -> IResult<MyInput<'_>, Vec<Output<'_>>> {
    let (i, _) = opt(hashtag_comment)(i)?;

    let (i, _) = tag(".inputs")(i)?;

    // let (i, _) = space1(i)?;
    // let (i, inputs) = separated_list0(tag(" \n\\"), alpha1)(i)?;
    // let (i, _) = line_ending(i)?;
    let (i, inputs) = blif_identifiers_list(i)?;

    Ok((i, inputs))
}

/// Parse the model line: eg `.outputs sum cout\n`
fn parse_outputs(i: MyInput<'_>) -> IResult<MyInput<'_>, Vec<Output<'_>>> {
    let (i, _) = opt(hashtag_comment)(i)?;

    let (i, _) = tag(".outputs")(i)?;

    let (i, outputs) = blif_identifiers_list(i)?;

    Ok((i, outputs))
}

/// Parse a gate line: eg `.gate XOR  a=b b=a O=new_n6_\n`
fn parse_gate(i: MyInput<'_>) -> IResult<MyInput<'_>, BlifGate> {
    let (i, _) = opt(hashtag_comment)(i)?;

    let (i, _) = tag(".gate")(i)?;

    let (i, _) = space1(i)?;
    let (i, gate_type) = alpha1(i)?;
    let (i, _) = space1(i)?;

    let (i, a) = opt(parse_gate_a)(i)?;

    let (i, b) = opt(parse_gate_b)(i)?;

    let (i, _) = tag("O=")(i)?;
    let (i, o) = blif_identifier(i)?;

    let (i, _) = line_ending(i)?;

    Ok((
        i,
        BlifGate {
            gate_type: gate_type.to_string(),
            a: a.map(Into::into),
            b: b.map(Into::into),
            o: o.into(),
        },
    ))
}

/// Parse the OPTIONAL "a=" or "b=" from a ".gate" line
fn parse_gate_a(input: MyInput<'_>) -> IResult<MyInput<'_>, Output<'_>> {
    let (i, _) = tag("a=")(input)?;
    let (i, b) = blif_identifier(i)?;
    let (i, _) = space1(i)?;

    Ok((i, b))
}
fn parse_gate_b(input: MyInput<'_>) -> IResult<MyInput<'_>, Output<'_>> {
    let (i, _) = tag("b=")(input)?;
    let (i, b) = blif_identifier(i)?;
    let (i, _) = space1(i)?;

    Ok((i, b))
}

/// Parse the last line: eg ".end\n"
fn parse_end(i: MyInput<'_>) -> IResult<MyInput<'_>, Output<'_>> {
    let (i, _) = opt(hashtag_comment)(i)?;

    let (i, _) = tag(".end")(i)?;
    let (i, _) = line_ending(i)?;

    Ok((i, i))
}

/// Parse a comment line starting with `# `
/// <https://github.com/rust-bakery/nom/blob/main/doc/nom_recipes.md#-ceol-style-comments>
/// and <https://stackoverflow.com/questions/76487424/how-to-count-comments-using-nom-parser>
fn hashtag_comment(i: MyInput<'_>) -> IResult<MyInput<'_>, Output<'_>> {
    let (i, _b) = value(
        (), // Output is thrown away.
        pair(char('#'), is_not("\n")),
    )(i)?;

    // let (a, b) = value(1, preceded(tag("#"), is_not("\n")))(i)?;

    let (i, _) = line_ending(i)?;

    Ok((i, i))
}

pub(crate) fn parse_blif(i: MyInput<'_>) -> IResult<MyInput<'_>, Blif> {
    let (i, _model) = parse_model(i)?;
    let (i, inputs) = parse_inputs(i)?;
    let (i, outputs) = parse_outputs(i)?;
    let (i, gates) = many1(parse_gate)(i)?;
    let (i, _) = parse_end(i)?;

    // convert the &str -> `BlifIdentifier`
    let inputs = inputs.into_iter().map(core::convert::Into::into).collect();
    let outputs = outputs.into_iter().map(core::convert::Into::into).collect();

    Ok((
        i,
        Blif {
            gates,
            inputs,
            outputs,
        },
    ))
}

impl TryFrom<Blif> for CircuitInternal {
    type Error = CircuitParserError;

    /// Try to convert a "raw .blif.blif" file(parsed with `nom` using functions above)
    /// into our internal struct `CircuitInternal`.
    ///
    /// The code is mostly copy-pasted from repo lib-garble-rs::/src/skcd_parser.rs
    ///
    fn try_from(circuit_blif: Blif) -> Result<Self, Self::Error> {
        let mut skcd_to_wire_ref_converter = SkcdToWireRefConverter::new();

        let mut inputs = Vec::with_capacity(circuit_blif.inputs.len());
        // This is for the "layers" conversion, see at the end
        let mut map_gate_id_to_layer = HashMap::new();
        for skcd_input in &circuit_blif.inputs {
            map_gate_id_to_layer.insert(skcd_input.id.clone(), 0);
        }

        // First loop on gates: we set only the output
        // That way we guarantee outputs are in order; we means we can directly write the outputs
        // of the eval (cf "main gate loop" in evaluate.rs in lib_garble_rs) without going through an intermediate Vec
        // TODO to allow parallisation: it SHOULD be loop on each gates layer; for this we need consecutive output INSIDE a layer, NOT globally
        // for skcd_gate in &circuit_blif.gates {
        //     // But `output` MUST always be set; this is what we use as Gate ID
        //     skcd_to_wire_ref_converter.insert(&skcd_gate.o.id);
        // }

        // First loop on gates: compute the layers
        // That way we guarantee outputs are in order; we means we can directly write the outputs
        // of the eval (cf "main gate loop" in evaluate.rs in lib_garble_rs) without going through an intermediate Vec
        //
        // Also transform Vec<Gates> -> Vec<Vec<Gates>>
        // cf `CircuitInternal` docstring
        //
        // A Gate's layer is the max(inputA's layer, inputB's layer)
        // WARNING The gates  ARE NOT guaranteed to be in topological order
        //
        // cf /circuit-gen-rs/tests/data/result_abc_full_adder_layers.blif.blif
        let mut gates_ids_layers = vec![];
        // for the next "gate loop" we need to go from `gates_ids_layers::Key` -> BlifGate
        let mut map_gate_id_to_blif_gate: HashMap<String, &BlifGate> = HashMap::new();
        // TODO to allow parallisation: it SHOULD be loop on each gates layer; for this we need consecutive output INSIDE alayer, NOT globally
        for skcd_gate in &circuit_blif.gates {
            map_gate_id_to_blif_gate.insert(skcd_gate.o.id.clone(), skcd_gate);

            let a_layer = if let Some(a) = &skcd_gate.a {
                map_gate_id_to_layer
                    .get(&a.id)
                    .ok_or(CircuitParserError::InvalidGateId {
                        gate_id: a.id.clone(),
                    })?
            } else {
                &0
            };

            let b_layer = if let Some(b) = &skcd_gate.b {
                map_gate_id_to_layer
                    .get(&b.id)
                    .ok_or(CircuitParserError::InvalidGateId {
                        gate_id: b.id.clone(),
                    })?
            } else {
                &0
            };

            // cf circuit-gen-rs/tests/data/result_abc_full_adder_layers.blif.blif for details
            // eg:
            // - if any/all gate inputs are circuit inputs -> current gate's layer is 0
            // - if at least one input is eg layer N (means it is the output of another gate) -> current gate's layer is 0
            // NOTE: but in `map_gate_id_to_layer` we insert the Gate's output so "+ 1"
            let o_layer = (a_layer + 1).max(b_layer + 1);
            map_gate_id_to_layer.insert(skcd_gate.o.id.clone(), o_layer);

            // Here we use the fact that the gates are guaranteed to be in topological order
            // so we can avoid iterating on the gates later and directly insert where it needs to be now
            //
            // We used "+ 1" b/c it is "output's layer" and we want "current gate layer" so compensate here
            let current_gate_layer = o_layer - 1;
            if gates_ids_layers.get(current_gate_layer).is_none() {
                gates_ids_layers.push(vec![]);
            }
            gates_ids_layers[current_gate_layer].push(skcd_gate.o.id.clone());
        }

        // CRITICAL me MUST do it BEFORE the inputs!
        for (layer_idx, layer_gate_ids) in gates_ids_layers.iter().enumerate() {
            for (layer_gate_idx, gate_id) in layer_gate_ids.iter().enumerate() {
                let skcd_gate: &&BlifGate = map_gate_id_to_blif_gate.get(gate_id).unwrap();

                // But `output` MUST always be set; this is what we use as Gate ID
                skcd_to_wire_ref_converter.insert(&skcd_gate.o.id);
            }
        }

        // Also CRITICAL we MUST do it before the main loop; but AFTER the outputs
        for skcd_input in &circuit_blif.inputs {
            skcd_to_wire_ref_converter.insert(&skcd_input.id);
            let gate_id = skcd_to_wire_ref_converter
                .get(&skcd_input.id)
                .ok_or_else(|| CircuitParserError::InvalidGateId {
                    gate_id: skcd_input.id.to_string(),
                })?;
            inputs.push(gate_id.clone());
        }

        // Convert each "Blif gate" into a proper `Gate`
        // The main idea here is to convert "Blif IDs"(== strings) into "proper IDs"(== usize)
        // The critical part is to understand that both "a" and "b" inputs are optional(zero, one or both can be set : Zero/One gates, Nor gates, etc)
        //
        let mut gates_layers: Vec<Vec<Gate>> = vec![];
        for (layer_idx, layer_gate_ids) in gates_ids_layers.iter().enumerate() {
            for (layer_gate_idx, gate_id) in layer_gate_ids.iter().enumerate() {
                let skcd_gate: &&BlifGate = map_gate_id_to_blif_gate.get(gate_id).unwrap();

                let output_wire_ref =
                    skcd_to_wire_ref_converter
                        .get(&skcd_gate.o.id)
                        .ok_or_else(|| CircuitParserError::OutputInvalidGateId {
                            gate_id: skcd_gate.o.id.clone(),
                        })?;

                // **IMPORTANT** NOT ALL Gate to be built require x_ref and y_ref
                // so DO NOT unwrap here!
                // That would break the circuit building process!
                let new_gate = match (&skcd_gate.a, &skcd_gate.b) {
                    (None, None) => Gate::new_from_skcd_gate_type(
                        &skcd_gate.gate_type,
                        output_wire_ref,
                        None,
                        None,
                    )?,
                    (None, Some(b)) => {
                        let y_ref = skcd_to_wire_ref_converter.get(&b.id).unwrap();
                        Gate::new_from_skcd_gate_type(
                            &skcd_gate.gate_type,
                            output_wire_ref,
                            None,
                            Some(y_ref),
                        )?
                    }
                    (Some(a), None) => {
                        let x_ref = skcd_to_wire_ref_converter.get(&a.id).unwrap();
                        Gate::new_from_skcd_gate_type(
                            &skcd_gate.gate_type,
                            output_wire_ref,
                            Some(x_ref),
                            None,
                        )?
                    }
                    (Some(a), Some(b)) => {
                        let x_ref = skcd_to_wire_ref_converter.get(&a.id).unwrap();
                        let y_ref = skcd_to_wire_ref_converter.get(&b.id).unwrap();
                        Gate::new_from_skcd_gate_type(
                            &skcd_gate.gate_type,
                            output_wire_ref,
                            Some(x_ref),
                            Some(y_ref),
                        )?
                    }
                };

                if gates_layers.get(layer_idx).is_none() {
                    gates_layers.push(vec![]);
                }
                gates_layers[layer_idx].push(new_gate);
            }
        }

        // IMPORTANT: we MUST use skcd.o to set the CORRECT outputs
        // eg for the 2 bits adder.skcd:
        // - skcd.m = 1
        // - skcd.o = [8,11]
        // -> the 2 CORRECT outputs to be set are: [8,11]
        // If we set the bad ones, we get "FancyError::UninitializedValue" in fancy-garbling/src/circuit.rs at "fn eval"
        // eg L161 etc b/c the cache is not properly set
        let mut outputs = Vec::with_capacity(circuit_blif.outputs.len());
        for skcd_output in &circuit_blif.outputs {
            skcd_to_wire_ref_converter.insert(&skcd_output.id);
            let output_wire_ref = skcd_to_wire_ref_converter
                .get(&skcd_output.id)
                .ok_or_else(|| CircuitParserError::OutputInvalidGateId {
                    gate_id: skcd_output.id.to_string(),
                })?
                .clone();
            outputs.push(output_wire_ref);
        }

        assert_eq!(
            circuit_blif.gates.len(),
            gates_layers.iter().map(alloc::vec::Vec::len).sum()
        );

        // CHECK: Gate IDs(== Gate OUTPUT ID) SHOULD match Gate INDEXES
        // CHECK:

        Ok(CircuitInternal {
            inputs,
            outputs,
            gates: gates_layers,
            wires: skcd_to_wire_ref_converter.get_all_wires(),
        })
    }
}

impl Circuit {
    /// Create a new "generic" circuit from a buffer containing a ".blif.blif"
    /// usually generated by the C++
    ///
    /// # Errors
    ///
    /// - if the buffer is not a valid ".blif.blif" file
    ///
    pub fn new_generic_from_blif_buffer(buf: &[u8]) -> Result<Self, CircuitParserError> {
        let buf_str =
            core::str::from_utf8(buf).map_err(|_| CircuitParserError::InvalidBufferNotUTF8)?;

        let (_, raw_blif) = crate::blif_parser::parse_blif(buf_str).map_err(|err| {
            CircuitParserError::BlifParserError {
                msg: err.to_string(),
            }
        })?;

        // Convert `Blif` -> `Circuit`
        let circuit: CircuitInternal = raw_blif.try_into()?;
        let metadata = Metadata::new(&circuit);

        Ok(Self {
            circuit,
            display_config: None,
            metadata,
        })
    }

    /// Create a new "display" circuit from a buffer containing a ".blif.blif"
    /// usually generated by the C++
    ///
    /// # Errors
    ///
    /// - if the buffer is not a valid ".blif.blif" file
    ///
    // TODO(lib-circuits-rs) DRY with `new_generic_from_blif_buffer`
    pub fn new_display_from_blif_buffer(
        buf: &[u8],
        has_watermark: bool,
        width: u32,
        height: u32,
        nb_segments: u32,
        rndsize: u32,
    ) -> Result<Self, CircuitParserError> {
        // the parsing in the same as the "generic" variant
        let mut circuit = Self::new_generic_from_blif_buffer(buf)?;

        // simply to "inject" the `CircuitDisplayConfig`
        circuit.display_config = Some(DisplayConfig::new(
            has_watermark,
            width,
            height,
            nb_segments,
            rndsize,
        ));

        Ok(circuit)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::GateType;
    use crate::GateType::Binary;
    use crate::KindBinary::*;
    use crate::WireRef;

    #[test]
    fn test_blif_identifier_internal() {
        for input in vec!["a", "full_add", "new_n8_"] {
            let (leftover_input, output) = blif_identifier_internal(input).unwrap();
            assert_eq!(leftover_input, "");
            // are the test inputs are valid IDs, so we should get the input value
            assert_eq!(output, input);
        }
    }

    #[test]
    fn test_blif_identifier() {
        for (input, expected_output) in vec![
            (" a", "a"),
            ("  full_add", "full_add"),
            ("\tnew_n8_", "new_n8_"),
            ("  \\ \n  \t  aaa", "aaa"),
            (" watmk[2200]", "watmk[2200]"),
        ] {
            let (leftover_input, output) = blif_identifier(input).unwrap();
            assert_eq!(leftover_input, "");
            assert_eq!(output, expected_output);
        }
    }

    #[test]
    fn test_blif_identifiers_list1() {
        let (leftover_input, output) = blif_identifiers_list(" a b\n").unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(output, vec!["a", "b"]);
    }

    #[test]
    fn test_parse_model() {
        let (leftover_input, model) = parse_model(".model full_add\n").unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(model, "full_add");
    }

    #[test]
    fn test_parse_inputs() {
        let (leftover_input, output) = parse_inputs(".inputs a b cin\n").unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(output, vec!["a", "b", "cin"]);
    }

    #[test]
    fn test_parse_inputs_multiline() {
        let (leftover_input, output) = parse_inputs(".inputs a b c \\\n   d e\n").unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(output, vec!["a", "b", "c", "d", "e"]);
    }

    #[test]
    fn test_parse_outputs() {
        let (leftover_input, output) = parse_outputs(
            ".outputs pix[0] pix[1] pix[2] pix[3] pix[4] pix[5] pix[6] pix[7] pix[8] \\\n
        pix[6232] pix[6233] pix[6234] pix[6235] pix[6236] pix[6237] pix[6238] \\\n
        pix[6239]\n",
        )
        .unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(
            output,
            vec![
                "pix[0]",
                "pix[1]",
                "pix[2]",
                "pix[3]",
                "pix[4]",
                "pix[5]",
                "pix[6]",
                "pix[7]",
                "pix[8]",
                "pix[6232]",
                "pix[6233]",
                "pix[6234]",
                "pix[6235]",
                "pix[6236]",
                "pix[6237]",
                "pix[6238]",
                "pix[6239]"
            ]
        );
    }

    #[test]
    fn test_parse_outputs_2() {
        let (leftover_input, output) = parse_outputs(".outputs sum cout\n").unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(output, vec!["sum", "cout"]);
    }

    #[test]
    fn test_parse_gate_binary() {
        let (leftover_input, output) = parse_gate(".gate XOR  a=aA b=bBb O=new_n6_\n").unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(
            output,
            BlifGate {
                gate_type: "XOR".to_string(),
                a: Some("aA").map(Into::into),
                b: Some("bBb").map(Into::into),
                o: "new_n6_".into(),
            }
        );
    }

    #[test]
    fn test_parse_gate_unary_buf() {
        let (leftover_input, output) =
            parse_gate(".gate BUF  a=watmk[2200] O=pix[2200]\n").unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(
            output,
            BlifGate {
                gate_type: "BUF".to_string(),
                a: Some("watmk[2200]").map(Into::into),
                b: None,
                o: "pix[2200]".into(),
            }
        );
    }

    #[test]
    fn test_parse_gate_unary_zero() {
        let (leftover_input, output) = parse_gate(".gate ZERO O=pix[6239]\n").unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(
            output,
            BlifGate {
                gate_type: "ZERO".to_string(),
                a: None,
                b: None,
                o: "pix[6239]".into(),
            }
        );
    }

    #[test]
    fn test_parse_end() {
        assert!(parse_end(".end\n").is_ok());
    }

    #[test]
    fn test_parse_blif_basic_full_adder() {
        let data = include_str!("../../circuit-gen-rs/tests/data/result_abc_full_adder.blif.blif");

        assert_eq!(
            parse_blif(data),
            Ok((
                "",
                Blif {
                    gates: vec![
                        BlifGate {
                            gate_type: "XOR".to_string(),
                            a: Some("b").map(Into::into),
                            b: Some("a").map(Into::into),
                            o: "new_n6_".into()
                        },
                        BlifGate {
                            gate_type: "XOR".to_string(),
                            a: Some("new_n6_").map(Into::into),
                            b: Some("cin").map(Into::into),
                            o: "sum".into()
                        },
                        BlifGate {
                            gate_type: "NAND".to_string(),
                            a: Some("b").map(Into::into),
                            b: Some("a").map(Into::into),
                            o: "new_n8_".into()
                        },
                        BlifGate {
                            gate_type: "NAND".to_string(),
                            a: Some("new_n6_").map(Into::into),
                            b: Some("cin").map(Into::into),
                            o: "new_n9_".into()
                        },
                        BlifGate {
                            gate_type: "NAND".to_string(),
                            a: Some("new_n9_").map(Into::into),
                            b: Some("new_n8_").map(Into::into),
                            o: "cout".into()
                        }
                    ],
                    inputs: vec!["a".into(), "b".into(), "cin".into()],
                    outputs: vec!["sum".into(), "cout".into()]
                }
            ))
        );
    }

    #[test]
    fn test_hashtag_comment() {
        let (leftover_input, output) = hashtag_comment(
            "# Benchmark \"full_add\" written by ABC on Tue Jul 25 12:08:48 2023\n",
        )
        .unwrap();
        assert_eq!(leftover_input, "");
        assert_eq!(output, "");
    }

    /// Same file than above `test_parse_blif_basic_full_adder` but with comments
    /// so we skip checking the value
    #[test]
    fn test_parse_blif_with_comments() {
        let data = include_str!(
            "../../circuit-gen-rs/tests/data/result_abc_full_adder_with_comments.blif.blif"
        );

        assert!(parse_blif(data).is_ok());
    }

    #[test]
    fn test_parse_blif_display_message_120x52_2digits() {
        let data = include_str!(
            "../../circuit-gen-rs/tests/data/result_display_message_120x52_2digits.blif.blif"
        );

        let circ = parse_blif(data);

        assert!(circ.is_ok());
        let (_, circ) = circ.unwrap();

        assert_eq!(circ.inputs.len(), 6395);
        assert_eq!(circ.outputs.len(), 120 * 52);
        assert_eq!(circ.gates.len(), 7874);
    }

    /// Mostly to test the `circuit.gates` == gate layers
    /// cf circuit-gen-rs/tests/data/result_abc_full_adder_layers.blif.blif
    /// For annotated comments
    #[test]
    fn test_try_from_circuit_internal() {
        let data = include_str!("../../circuit-gen-rs/tests/data/result_abc_full_adder.blif.blif");
        let circ = parse_blif(data);
        assert!(circ.is_ok());
        let (_, raw_blif) = circ.unwrap();

        // Convert `Blif` -> `Circuit`
        let circuit: CircuitInternal = raw_blif.try_into().unwrap();

        assert_eq!(
            circuit.gates,
            vec![
                vec![
                    // O=new_n6_
                    Gate {
                        internal: Binary {
                            gate_type: XOR,
                            input_a: WireRef { id: 6 },
                            input_b: WireRef { id: 5 }
                        },
                        output: WireRef { id: 0 }
                    },
                    // O=new_n8_
                    Gate {
                        internal: Binary {
                            gate_type: NAND,
                            input_a: WireRef { id: 6 },
                            input_b: WireRef { id: 5 }
                        },
                        output: WireRef { id: 1 }
                    }
                ],
                vec![
                    // O=sum
                    Gate {
                        internal: Binary {
                            gate_type: XOR,
                            input_a: WireRef { id: 0 },
                            input_b: WireRef { id: 7 }
                        },
                        output: WireRef { id: 2 }
                    },
                    // O=new_n9_
                    Gate {
                        internal: Binary {
                            gate_type: NAND,
                            input_a: WireRef { id: 0 },
                            input_b: WireRef { id: 7 }
                        },
                        output: WireRef { id: 3 }
                    }
                ],
                // O=cout
                vec![Gate {
                    internal: Binary {
                        gate_type: NAND,
                        input_a: WireRef { id: 3 },
                        input_b: WireRef { id: 1 }
                    },
                    output: WireRef { id: 4 }
                }]
            ]
        );
        assert_eq!(
            circuit.wires,
            vec![
                WireRef { id: 0 },
                WireRef { id: 1 },
                WireRef { id: 2 },
                WireRef { id: 3 },
                WireRef { id: 4 },
                WireRef { id: 5 },
                WireRef { id: 6 },
                WireRef { id: 7 }
            ]
        );
        assert_eq!(
            circuit.inputs,
            vec![WireRef { id: 5 }, WireRef { id: 6 }, WireRef { id: 7 }]
        );
        assert_eq!(circuit.outputs, vec![WireRef { id: 2 }, WireRef { id: 4 }]);
    }
}
