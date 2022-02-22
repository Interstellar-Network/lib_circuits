
#include "blif_parser.h"

#include <absl/strings/str_split.h>
#include <fmt/format.h>
#include <xxhash.h>

#include <iostream>
#include <stack>
#include <unordered_set>

static constexpr char kdummy0[] = "__dummy0";
static constexpr char kdummy1[] = "__dummy1";

namespace interstellar {

// TODO(unordered_map?)
std::map<std::string_view, SkcdGateType> kMapStringGatetype = {
    {"ZERO", SkcdGateType::ZERO}, {"NOR", SkcdGateType::NOR},
    {"AANB", SkcdGateType::AANB}, {"INVB", SkcdGateType::INVB},
    {"NAAB", SkcdGateType::NAAB}, {"INV", SkcdGateType::INV},
    {"XOR", SkcdGateType::XOR},   {"NAND", SkcdGateType::NAND},
    {"AND", SkcdGateType::AND},   {"XNOR", SkcdGateType::XNOR},
    {"BUF", SkcdGateType::BUF},   {"AONB", SkcdGateType::AONB},
    {"BUFB", SkcdGateType::BUFB}, {"NAOB", SkcdGateType::NAOB},
    {"OR", SkcdGateType::OR},     {"ONE", SkcdGateType::ONE}};

namespace {

class Gate {
 public:
  SkcdGateType type;

  // a and/or b can be null
  // o is always set
  std::string a, b, o;

  GateLayer layer;
};

/**
 * Parse a '.gate' line in the .blif.blif
 * eg:
 * .gate ZERO O=pix[117713]
 * .gate INV  a=z O=n61356
 * .gate XOR  a=n152079 b=n137910 O=pix[36483]
 * etc
 */
// TODO better parser; at least use https://abseil.io/docs/cpp/guides/strings
// for comparison etc
// or use lorina?
Gate ParseGateLine(const std::string_view &gate_line,
                   const std::unordered_set<std::string_view> &inputs_set,
                   CircuitData *circuit_data) {
  std::vector<std::string_view> gate_vect = absl::StrSplit(gate_line, ' ');

  assert(gate_vect[0] == ".gate");  // NOT a .gate line

  Gate gate;

  // TODO(string_view) remove string copy
  std::string gate_vect_type_copy;
  gate_vect_type_copy.resize(gate_vect[1].size());
  gate_vect[1].copy(gate_vect_type_copy.data(), gate_vect[1].size());

  gate.type = kMapStringGatetype.at(gate_vect_type_copy);

  size_t gate_vect_size = gate_vect.size();

  // we have already processed [0](ie ".gate"), and [1](ie the gate type)
  for (unsigned int i = 2; i < gate_vect_size; i++) {
    // eg: a=z, b=rnd[0], O=n249
    const std::string_view &sub_gate = gate_vect[i];

    assert(sub_gate[1] == '=');  // invalid token

    // eg O=pix[83276]  --> sub_gate_name_copy = pix[83276]
    // TODO(string_view) remove string copy
    std::string sub_gate_name_copy;
    sub_gate_name_copy.resize(sub_gate.size() - 2);
    sub_gate.copy(sub_gate_name_copy.data(), sub_gate.size(), 2);

    if (sub_gate[0] == 'a') {
      gate.a = sub_gate_name_copy;
    } else if (sub_gate[0] == 'b') {
      gate.b = sub_gate_name_copy;
    } else if (sub_gate[0] == 'O') {
      gate.o = sub_gate_name_copy;
    } else {
      printf("ERROR: BlifParser::ParseGateLine error 2: could not parse : %s\n",
             sub_gate.data());
      throw std::runtime_error(
          "BlifParser::ParseGateLine : could not parse the .gate");
    }
  }

  // Now we can compute the gate's layer
  // 1: if a gate has two inputs
  const auto end = inputs_set.end();
  if (!gate.a.empty() && !gate.b.empty()) {
    // case: both inputs are ROOT
    // eg: '.gate XOR  a=rnd[2] b=rnd[0] O=n29824'
    if (inputs_set.find(gate.a) != end && inputs_set.find(gate.b) != end) {
      gate.layer = GateLayer::ROOT;
    }
    // case: only ONE of the input is ROOT
    // eg: '.gate NAND a=n31928 b=msg[6] O=n31929'
    else if (inputs_set.find(gate.a) != end || inputs_set.find(gate.b) != end) {
      gate.layer = GateLayer::INTERMEDIATE;
    }
    // case: both inputs are NOT ROOT
    // eg: '.gate XOR  a=n41064 b=n29793 O=pix[13856]'
    else {
      gate.layer = GateLayer::SECOND;
    }
  } else {
    // In pratice this is the two kind of gate remaining to parse:
    // '.gate ZERO O=pix[18286]'(A LOT of them)
    // '.gate INV  a=z O=n29793'(only one)
    assert(
        (gate.type == SkcdGateType::ZERO || gate.type == SkcdGateType::INV) &&
        "ParseGateLine: unexpected: not a ZERO or INV!");
    if (gate.type == SkcdGateType::ZERO) {
      // ZERO gates are transformed into XOR(__dummy0, __dummy0)
      // Which are two ROOT inputs
      gate.layer = GateLayer::ROOT;
    } else {
      // INV gates are transformed into XOR(a, __dummy1)
      // So either 'a' is a ROOT input which makes the gate ROOT,
      // or 'a' is INTERMEDIATE, which the the gate INTERMEDIATE
      assert(!gate.a.empty() && "INV gate without 'a' set!");
      if (inputs_set.find(gate.a) != end) {
        gate.layer = GateLayer::ROOT;
      } else {
        gate.layer = GateLayer::INTERMEDIATE;
      }
    }
  }

  // update the layer's count
  assert(static_cast<uint8_t>(gate.layer) < circuit_data->layer_count.size() &&
         "ParseGateLine: gate.layer: out of range!");
  circuit_data->layer_count[static_cast<uint8_t>(gate.layer)]++;

  return gate;
}

}  // namespace

/**
 * NOTE: as this is using string_views,
 * you MUST keep blif_buffer alive until you are done with this instance
 * ie until after ToSkcd has been called.
 */
// TODO handle the '-z' option, see lib_python
// TODO return a struct/class
void BlifParser::ParseBuffer(const std::string &blif_buffer, bool zero) {
  assert(zero && "BlifParser: only 'zero'(=-z flag) is supported!");

  std::vector<std::string_view> lines;

  // IMPORTANT: we are using string_views, but to handle the continuations, we
  // need some form of concat, so we use a string(a sv is necessarilt
  // contiguous) Therefore, we must have a "buffer" that extends the inner
  // loop[its lifetime must be greater than the last access to 'lines'] NOTE:
  // there is typically only a few "continuation" block in a .blif.blif so even
  // if we do a few str copies, this is no big deal(opposed to 50-60k lines in
  // the .blif.blif)
  std::stack<std::unique_ptr<std::string>> continuation_buffers;

  {
    // std::istringstream blif_ss(blif_buffer);
    std::string_view blif_sv{blif_buffer.c_str(), blif_buffer.size()};

    std::vector<std::string_view> blif_lines_raw =
        absl::StrSplit(blif_sv, '\n');

    // Pre-processing
    // - remove comments
    // - handle line continuation : if '\' at end of line -> append to previous
    // line
    bool handling_continuation = false;

    continuation_buffers.push(std::make_unique<std::string>());
    std::string *line_with_continuation = continuation_buffers.top().get();

    size_t blif_lines_raw_size = blif_lines_raw.size();
    for (unsigned int i = 0; i < blif_lines_raw_size; ++i) {
      std::string_view &line = blif_lines_raw[i];

      size_t line_length = line.length();

      // just in case, skip empty lines
      if (line_length == 0u) {
        continue;
      }

      if (line.at(0) == '#') {
        // comment : skip
        // NOTE: only handles a "header" comment, but we don't need more
        continue;
      }

      if (line.at(line_length - 1) == '\\') {
        *line_with_continuation += line.substr(0, line_length - 2);

        if (handling_continuation) {
          // In the continuation group
        } else {
          // First line in the continuation group
          handling_continuation = true;
        }

      } else {
        if (handling_continuation) {
          // Last line in the continuation group
          handling_continuation = false;

          *line_with_continuation += line;

          lines.push_back(*line_with_continuation);

          // reset the line_with_continuation buffer
          continuation_buffers.push(std::make_unique<std::string>());
          line_with_continuation = continuation_buffers.top().get();
        } else {
          // Standard line
          lines.push_back(line);
        }
      }
    }
  }

  size_t nb_lines = lines.size();

  printf("parsed : %zu lines\n", nb_lines);

  // First line
  // expected : .model main
  // TODO technically we only care about ".model"; "main" is making the parser
  // less flexible for no good reason
  if (lines[0] != ".model main") {
    throw std::runtime_error("BlifParser::Parse : invalid .model");
  }

  // Second line
  // expected: .input msg[0] msg[1] etc
  if (lines[1].find(".inputs") == std::string::npos) {
    throw std::runtime_error("BlifParser::Parse : expected .inputs");
  }

  std::vector<std::string_view> inputs_vect = absl::StrSplit(lines[1], ' ');

  // don't count ".inputs";
  // TODO remove this and skip the first when iterating (it's causing a realloc
  // of the storage)
  inputs_vect.erase(inputs_vect.begin());

  n_ = inputs_vect.size();

  // Init the labels
  for (auto &input : inputs_vect) {
    GetLabel(input);
  }

  // Put the inputs into a set
  // We use it to compute on which layer a gate is(in ParseGateLine)
  std::unordered_set<std::string_view> inputs_set(inputs_vect.begin(),
                                                  inputs_vect.end());

  // Third line
  // expected: .outputs pix[0] pix[1] etc
  if (lines[2].find(".outputs") == std::string::npos) {
    throw std::runtime_error("BlifParser::Parse : expected .outputs");
  }

  std::vector<std::string_view> outputs_vect = absl::StrSplit(lines[2], ' ');

  // don't count ".outputs";
  // TODO remove this and skip the first when iterating (it's causing a realloc
  // of the storage)
  outputs_vect.erase(outputs_vect.begin());

  m_ = outputs_vect.size();

  // Based on "zero" or not, we will handle the .gate differently
  // Step 1: parse all the gates
  // NOTE: excluding the last one, which is ".endmodule"
  std::vector<Gate> gates_vect;
  size_t count0 = 0, count1 = 0;  // only needed when "zero"
  for (auto line_gate = lines.begin() + 3; line_gate != lines.end() - 1;
       ++line_gate) {
    Gate gate = ParseGateLine(*line_gate, inputs_set, &circuit_data_);
    gates_vect.push_back(gate);

    if ((gate.type == SkcdGateType::ZERO) || (gate.type == SkcdGateType::BUF)) {
      count0++;
    } else if ((gate.type == SkcdGateType::ONE) ||
               (gate.type == SkcdGateType::INV)) {
      count1++;
    }
  }

  // only when "zero"
  std::vector<unsigned int> pool0, pool1;  // vector of labels
  unsigned int ng =
      std::max(static_cast<unsigned int>(std::ceil(std::sqrt(count0))),
               static_cast<unsigned int>(std::ceil(std::sqrt(count1))));

  // Could refactor this to a max + add ng%2
  if (ng < 2) {
    ng = 2;
  }
  if ((ng % 2) != 0u) {
    ng++;
  }

  // prealloc
  // A,B,GO,GT: when 'zero':
  // - two 'push_back' 'ng' times
  // - one 'push_back' per gates_vect elem
  size_t size_to_reserve = 2 * ng + gates_vect.size();
  A_.reserve(size_to_reserve);
  B_.reserve(size_to_reserve);
  GO_.reserve(size_to_reserve);
  GT_.reserve(size_to_reserve);

  if (!zero) {
    // allocate 2 dummies
    // NOTE: we do it now to have the same behavior as lib_python, but it's not
    // required it's just this way it's easier to compare the lib_python
    // implementation and this one(unit test, etc)
    GetLabel(kdummy0);
    GetLabel(kdummy1);
  } else {
    // allocate gates of type ZERO and ONE for dummies
    // TODO DRY

    // "SUMMARY: AddressSanitizer: stack-use-after-scope"
    // Related to eg GetLabel( "__dummy0") line 264
    // we MUST keep the "__dummyN" strings valid for this whole function

    pool0.reserve(ng);

    for (unsigned int i = 0; i < ng; i++) {
      A_.push_back(0);
      B_.push_back(0);
      GT_.push_back(SkcdGateType::ZERO);
      unsigned int lbl = GetLabel("__dummy" + fmt::format_int(i * 2).str());
      pool0.push_back(lbl);
      GO_.push_back(lbl);
      q_++;
    }

    pool1.reserve(ng);

    for (unsigned int i = 0; i < ng; i++) {
      A_.push_back(0);
      B_.push_back(0);
      GT_.push_back(SkcdGateType::ONE);
      unsigned int lbl = GetLabel("__dummy" + fmt::format_int(i * 2 + 1).str());
      pool1.push_back(lbl);
      GO_.push_back(lbl);
      q_++;
    }

    // the __dummyN are root inputs, so add them to the count to make sure
    // circuit_data's layer_count matches the final gate's count(ie 'q_')
    circuit_data_.layer_count[static_cast<uint8_t>(GateLayer::ROOT)] += ng * 2;
  }

  printf("BlifParser: inputs : %d, outputs : %d\n", n_, m_);

  // Now the main part: the ".gate"
  // Contrary to lib_python reuse the parsed gates
  // For later use(ie garbling) we also compute the min/max of the gate inputs
  // and gate outputs
  for (auto &gate : gates_vect) {
    assert(gate.o.length());  // gate's O not set

    // contrary to 'O', a & b are optionnal
    if (!gate.a.empty()) {
      auto gate_a = A_.emplace_back(GetLabel(gate.a));

      circuit_data_.gate_input_min =
          std::min(gate_a, circuit_data_.gate_input_min);
      circuit_data_.gate_input_max =
          std::max(gate_a, circuit_data_.gate_input_max);
    } else {
      A_.push_back(GetLabel(kdummy0));
    }

    if (!gate.b.empty()) {
      auto gate_b = B_.emplace_back(GetLabel(gate.b));

      circuit_data_.gate_input_min =
          std::min(gate_b, circuit_data_.gate_input_min);
      circuit_data_.gate_input_max =
          std::max(gate_b, circuit_data_.gate_input_max);
    } else {
      B_.push_back(GetLabel(kdummy0));
    }

    auto gate_o = GO_.emplace_back(GetLabel(gate.o));
    GT_.push_back(gate.type);

    circuit_data_.gate_output_min =
        std::min(gate_o, circuit_data_.gate_output_min);
    circuit_data_.gate_output_max =
        std::max(gate_o, circuit_data_.gate_output_max);

    if (zero) {
      if (gate.type == SkcdGateType::ZERO) {
        // "replace ZERO by XOR(__dummy0, __dummy0)=0"
        GT_[q_] = SkcdGateType::XOR;

        std::vector<unsigned int> &pool =
            random_->randint(0, 1) != 0u ? pool0 : pool1;

        unsigned int x = random_->randint(0, ng / 2 - 1);

        A_[q_] = pool.at(2 * x);
        B_[q_] = pool.at(2 * x + 1);

      } else if (gate.type == SkcdGateType::ONE) {
        // "replace ONE by XOR(__dummy0, __dummy1)=1"
        GT_[q_] = SkcdGateType::XOR;

        unsigned int x = random_->randint(0, ng - 1);

        if (random_->randint(0, 1) != 0u) {
          A_[q_] = pool0[x];
          B_[q_] = pool1[x];
        } else {
          A_[q_] = pool1[x];
          B_[q_] = pool0[x];
        }

      } else if (gate.type == SkcdGateType::BUF) {
        // "replace BUF by XOR(a, __dummy0)=a"
        GT_[q_] = SkcdGateType::XOR;

        unsigned int random_choice = random_->choice(pool0);
        B_[q_] = random_choice;

      } else if (gate.type == SkcdGateType::INV) {
        // "replace INV by XOR(a, __dummy1)=not a"
        GT_[q_] = SkcdGateType::XOR;

        unsigned int random_choice = random_->choice(pool1);
        B_[q_] = random_choice;
      }
    }

    q_++;
  }

  O_.reserve(outputs_vect.size());

  for (auto &output : outputs_vect) {
    O_.push_back(GetLabel(output));
  }

  assert(A_.size() == size_to_reserve && "BlifParser: A was resized!");
  assert(B_.size() == size_to_reserve && "BlifParser: B was resized!");
  assert(GO_.size() == size_to_reserve && "BlifParser: GO was resized!");
  assert(GT_.size() == size_to_reserve && "BlifParser: GT was resized!");
  assert(O_.size() == outputs_vect.size() && "BlifParser: O was resized!");

  // CHECK layer_count must match the number of gate = every gate should have
  // been counted
  assert(std::accumulate(circuit_data_.layer_count.begin(),
                         circuit_data_.layer_count.end(), 0u) == q_ &&
         "'q' does not match layer_count!");

  // circuit_data: update input -> gate map
  // we init it we another loop to have the correct final A & B
  // (else we would miss some __dummyN in the counts)
  // Also this way we can init to 'just the size we need' instead of 'nb_labels'
  // gate_input_max + 1 b/c gate_input_max IS a valid input
  circuit_data_.input_gate_count.resize(circuit_data_.gate_input_max + 1, 0);
  for (uint32_t i = 0; i < q_; ++i) {
    circuit_data_.input_gate_count[A_[i]]++;
    circuit_data_.input_gate_count[B_[i]]++;
  }
}

/**
 * Return the corresponding 'label', as an 'int'(ie basically an index).
 *
 * Comparison of std::hash and XXHash:
 *  ## std::hash
    refs:      7,362,209,105
    refs:      7,363,205,647

    ## labels_map_: XXHash
    refs:      7,386,706,431
    refs:      7,386,706,431

    So use std::hash.
 */
uint32_t BlifParser::GetLabel(std::string_view lbl) {
  // TODO instead of hashing manually we should pass the hash function when
  // declaring labels_map_
  // TODO seed
  uint64_t lbl_hash = XXH64(lbl.data(), lbl.size(), 4242);

  auto label_idx = labels_map_.find(lbl_hash);

  if (label_idx != labels_map_.end()) {
    return (*label_idx).second;
  }
  // current_label_count_++;
  // labels_map_.insert(std::pair<std::string, size_t>(lbl,
  // current_label_count_)); return current_label_count_;
  /**
  number = len(labels)
  labels[label] = number
  return number
  */
  labels_map_.insert(std::pair<uint64_t, size_t>(lbl_hash, labels_map_.size()));
  assert(labels_map_.size() < std::numeric_limits<uint32_t>::max() &&
         "BlifParser: label does not fit on 32 bits!");
  return labels_map_.size() - 1;
}

}  // namespace interstellar