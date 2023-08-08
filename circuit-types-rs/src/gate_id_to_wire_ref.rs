use hashbrown::HashMap;

#[cfg(feature = "alloc")]
use alloc::string::String;
#[cfg(feature = "alloc")]
use alloc::string::ToString;
#[cfg(feature = "alloc")]
use alloc::vec::Vec;

use super::WireRef;

/// We need to convert something like
/// ".gate XOR  a=rnd[2] b=rnd[0] O=n7016" in the .skcd(which is basically a .blif)
/// into something that `CircuitBuilder` can accept.
/// Essentially we need to convert a String ID -> `CircuitRef`(= a usize)
///
/// IMPORTANT
/// For this to work, the INPUTS MUST also go through the same conversion, else
/// when using CircuitBuilder.or/and/etc the `CircuitRef` WOULD NOT match anything.
/// NOTE that in this case the Circuit still would build fine, but it would fail
/// when eval/garbling.
#[derive(Debug, Clone)]
pub(crate) struct SkcdToWireRefConverter {
    map_skcd_gate_id_to_circuit_ref: HashMap<String, WireRef>,
    cur_len: usize,
}

impl SkcdToWireRefConverter {
    pub(crate) fn new() -> Self {
        Self {
            map_skcd_gate_id_to_circuit_ref: HashMap::new(),
            cur_len: 0,
        }
    }

    pub(crate) fn get(&self, skcd_gate_id: &str) -> Option<&WireRef> {
        self.map_skcd_gate_id_to_circuit_ref.get(skcd_gate_id)
    }

    /// insert
    /// NOOP if already in the map
    pub(crate) fn insert(&mut self, skcd_gate_id: &str) {
        if self.get(skcd_gate_id).is_none() {
            self.map_skcd_gate_id_to_circuit_ref
                .insert(skcd_gate_id.to_string(), WireRef { id: self.cur_len });
            self.cur_len += 1;
        }
    }

    /// Return the ORDERED list of all the wires.
    /// Used during garbling "init" function to create the "encoding".
    /// WARNING: this calls "`into_values`" so the `SkcdToWireRefConverter` CAN NOT be used afterward!
    ///
    /// The ORDERING is CRITICAL (for now).
    /// Technically we should probably get away with splitting the wires in input+gates+outputs
    /// and keep the ordering b/w them but not internally?
    pub(crate) fn get_all_wires(self) -> Vec<WireRef> {
        let mut wires: Vec<WireRef> = self.map_skcd_gate_id_to_circuit_ref.into_values().collect();
        wires.sort_by(|a, b| a.id.cmp(&b.id));
        wires
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_skcd_to_wire_ref_converter_stable() {
        let mut converter = SkcdToWireRefConverter::new();

        let gate_id = "42";
        converter.insert(gate_id);
        let a = converter.get(gate_id).unwrap().clone();
        converter.insert(gate_id);
        let b = converter.get(gate_id).unwrap().clone();

        assert_eq!(a, b);
    }

    #[test]
    fn test_skcd_to_wire_ref_converter_get_all_wires_is_ordered() {
        let mut converter = SkcdToWireRefConverter::new();

        let test_gates_ids = ["a", "42", "0", "azerty", "1", "dgfg", "353.12"];

        for (idx, test_gates_id) in test_gates_ids.iter().enumerate() {
            converter.insert(test_gates_id);
            let wire_ref = converter.get(test_gates_id).unwrap();
            assert_eq!(wire_ref.id, idx, "unexpected internal ID!");
        }

        assert_eq!(
            converter
                .get_all_wires()
                .iter()
                .map(|w| w.id)
                .collect::<Vec<_>>(),
            vec![0, 1, 2, 3, 4, 5, 6]
        );
    }
}
