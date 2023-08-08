use hashbrown::{HashMap, HashSet};
use serde::{Deserialize, Serialize};

use crate::{CircuitInternal, GateType, KindBinary, KindUnary, WireRef};

/// Various stats, min/max indexes for other fields, etc
/// This is useful both for debugging/info, and for eg optimising alloc
#[derive(Serialize, Deserialize, PartialEq, Debug, Clone)]
pub struct Metadata {
    /// When garbling and evaluating, we iterate on the `gates` in topological order
    /// and we need to know if a gate is an output to perform special treatment on it.
    /// So instead of having to look-up inside `outputs`(or a temp corresponding hashset), we directly
    /// store the indices.
    /// NOTE: these are essentially INDICES refering to `gates`
    outputs_start_end_indexes: (usize, usize),
    // BELOW fields are mostly for Debug/Stats/etc
    gates_unary_count: HashMap<KindUnary, usize>,
    gates_binary_count: HashMap<KindBinary, usize>,
    gates_0_count: usize,
    gates_1_count: usize,
    /// The max gate.get_id() we can find in circuit.gates(typically get_id() == gate.output)
    /// We need this to init the proper vec to store "deltas"
    max_gate_id: usize,
}

impl Metadata {
    /// Build a new `CircuitMetadata` from a `Circuit`
    ///
    /// NOTE: we COULD optimize and do this in eg `blif_parser.rs` but for simplicity/clarity
    /// we do it separately.
    /// And anyway this is only called "once in a while" when generating circuits so performance
    /// does not really matter.
    ///
    /// The code mostly comes from <https://github.com/Interstellar-Network/lib-garble-rs/blob/c0d249ba4696d63d26da199e3bf58252baa59c5a/lib-garble-rs/src/skcd_parser.rs#L74>
    ///
    pub(crate) fn new(circuit: &CircuitInternal) -> Self {
        // `outputs_start_end_indexes` after only works if `outputs` are consecutive; so CHECK it!
        // https://stackoverflow.com/questions/59028400/comparing-every-element-in-a-vector-with-the-next-one
        assert!(
            circuit.outputs.windows(2).all(|w| w[1].id == w[0].id + 1),
            "non consecutive elements in `outputs`!"
        );

        let outputs_set: HashSet<&WireRef> = circuit.outputs.iter().collect();

        let mut metadata = Self {
            outputs_start_end_indexes: (usize::MAX, usize::MIN),
            gates_unary_count: HashMap::new(),
            gates_binary_count: HashMap::new(),
            gates_0_count: 0,
            gates_1_count: 0,
            max_gate_id: usize::MIN,
        };

        for gate in &circuit.gates {
            let output_wire_ref = &gate.output;

            // if gate is a circuit output wire then update the min/max indices
            if outputs_set.contains(output_wire_ref) {
                metadata.outputs_start_end_indexes.0 =
                    metadata.outputs_start_end_indexes.0.min(output_wire_ref.id);
                metadata.outputs_start_end_indexes.1 =
                    metadata.outputs_start_end_indexes.1.max(output_wire_ref.id);
            }
            metadata.max_gate_id = metadata.max_gate_id.max(output_wire_ref.id);

            metadata.increment_gate_type(gate.get_type());
        }

        metadata
    }

    /// [TEST ONLY] Useful when creating test circuits manually in unit tests
    #[doc(hidden)]
    pub(crate) fn new_for_test(
        outputs_start_end_indexes: (usize, usize),
        max_gate_id: usize,
    ) -> Self {
        Self {
            outputs_start_end_indexes,
            gates_unary_count: HashMap::new(),
            gates_binary_count: HashMap::new(),
            gates_0_count: 0,
            gates_1_count: 0,
            max_gate_id,
        }
    }

    fn increment_gate_type(&mut self, gate_type: &GateType) {
        match gate_type {
            GateType::Binary {
                gate_type,
                input_a: _,
                input_b: _,
            } => {
                self.gates_binary_count
                    .entry(gate_type.clone())
                    .and_modify(|count| *count += 1)
                    .or_insert(1);
            }
            GateType::Unary {
                gate_type,
                input_a: _,
            } => {
                self.gates_unary_count
                    .entry(gate_type.clone())
                    .and_modify(|count| *count += 1)
                    .or_insert(1);
            }
            GateType::Constant { value } => match value {
                true => self.gates_1_count += 1,
                false => self.gates_0_count += 1,
            },
        };
    }

    /// param `idx`: SHOULD be an index from `CircuitInternal.gates`
    ///
    /// Used by repo [lib-garble-rs], not internally here!
    ///
    #[must_use]
    pub fn gate_idx_is_output(&self, idx: usize) -> bool {
        (idx >= self.outputs_start_end_indexes.0) && (idx <= self.outputs_start_end_indexes.1)
    }

    /// When building `output_labels`, we need to insert elements base on
    /// the current gate ID; but in `decoding_internal` we directly loop on the outputs, NOT on the gates.
    /// So we need to map eg "gate ID" vs "output index"
    ///
    /// Used by repo [lib-garble-rs], not internally here!
    ///
    #[must_use]
    pub fn convert_gate_id_to_outputs_index(&self, id: usize) -> usize {
        id - self.outputs_start_end_indexes.0
    }

    ///
    /// Used by repo [lib-garble-rs], not internally here!
    ///
    #[must_use]
    pub fn get_max_gate_id(&self) -> usize {
        self.max_gate_id
    }
}
