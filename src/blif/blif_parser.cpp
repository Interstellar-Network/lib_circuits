// Copyright 2022 Nathan Prat

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "blif_parser.h"

#include <absl/random/random.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_split.h>
#include <fmt/format.h>
#include <xxhash.h>

#include <cassert>
#include <iostream>
#include <stack>

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
Gate ParseGateLine(std::string_view gate_line) {
  std::vector<std::string_view> gate_vect =
      absl::StrSplit(gate_line, ' ', absl::SkipEmpty());

  if (gate_vect[0] != ".gate") {
    throw std::runtime_error("BlifParser: NOT a .gate line!");
  }

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
    std::string_view sub_gate = gate_vect[i];

    if (sub_gate[1] != '=') {
      throw std::runtime_error("BlifParser: invalid token!");
    }

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

  return gate;
}

}  // namespace

BlifParser::BlifParser() {}

void BlifParser::ParseBuffer(std::string_view blif_buffer, bool zero) {
  absl::BitGen bitgen;
  ParseBuffer(blif_buffer, zero, bitgen);
}

/**
 * NOTE: as this is using string_views,
 * you MUST keep blif_buffer alive until you are done with this instance
 * ie until after ToSkcd has been called.
 *
 * param bitgen: DEV/TEST
 * Used during tests to use a fake PRNG, needed to have a determistic output
 * of Parse*() with "zero=True"
 * TODO remove when NOT a test build
 */
// TODO handle the '-z' option, see lib_python
// TODO return a struct/class
void BlifParser::ParseBuffer(std::string_view blif_buffer, bool zero,
                             absl::BitGenRef bitgen) {
  if (!zero) {
    throw std::runtime_error("BlifParser: only 'zero'(=-z flag) is supported!");
  }

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
    std::vector<std::string_view> blif_lines_raw =
        absl::StrSplit(blif_buffer, '\n');

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

      if (absl::StartsWith(line, "#")) {
        // comment : skip
        // NOTE: only handles a "header" comment, but we don't need more
        continue;
      }

      // if a line ends with "\", concat with the next one
      if (absl::EndsWith(line, "\\")) {
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

  // First line
  // expected : .model main
  // TODO technically we only care about ".model"; "main" is making the parser
  // less flexible for no good reason
  if (!absl::StartsWith(lines[0], ".model ")) {
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
    Gate gate = ParseGateLine(*line_gate);
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
  }

  assert(pool0.size() == ng && "pool0: wrong size!");
  assert(pool1.size() == ng && "pool0: wrong size!");

  bool rand_switch = absl::Bernoulli(bitgen, 0.5);

  // Now the main part: the ".gate"
  // Contrary to lib_python reuse the parsed gates
  // For later use(ie garbling) we also compute the min/max of the gate inputs
  // and gate outputs
  for (auto &gate : gates_vect) {
    if (!gate.o.length()) {
      throw std::runtime_error("gate's O not set");
    }

    // contrary to 'O', a & b are optionnal
    if (!gate.a.empty()) {
      A_.emplace_back(GetLabel(gate.a));
    } else {
      A_.push_back(GetLabel(kdummy0));
    }

    if (!gate.b.empty()) {
      B_.emplace_back(GetLabel(gate.b));
    } else {
      B_.push_back(GetLabel(kdummy0));
    }

    GO_.emplace_back(GetLabel(gate.o));
    GT_.push_back(gate.type);

    if (zero) {
      if (gate.type == SkcdGateType::ZERO) {
        // "replace ZERO by XOR(__dummy0, __dummy0)=0"
        GT_[q_] = SkcdGateType::XOR;

        std::vector<unsigned int> &pool = rand_switch != 0u ? pool0 : pool1;

        unsigned int x = absl::Uniform<uint32_t>(bitgen, 0, ng / 2 - 1);

        A_[q_] = pool.at(2 * x);
        B_[q_] = pool.at(2 * x + 1);

      } else if (gate.type == SkcdGateType::ONE) {
        // "replace ONE by XOR(__dummy0, __dummy1)=1"
        GT_[q_] = SkcdGateType::XOR;

        unsigned int x = absl::Uniform<uint32_t>(bitgen, 0, ng - 1);

        if (rand_switch) {
          A_[q_] = pool0[x];
          B_[q_] = pool1[x];
        } else {
          A_[q_] = pool1[x];
          B_[q_] = pool0[x];
        }

      } else if (gate.type == SkcdGateType::BUF) {
        // see comment "BUF gates are to be duplicated" above
        // replace BUF gate are to be duplicated by or O = BUF(A) <=> O =
        // __dummy0 XOR A
        GT_[q_] = SkcdGateType::XOR;

        unsigned int random_choice =
            pool0[absl::Uniform<uint32_t>(bitgen, 0, pool0.size())];
        B_[q_] = random_choice;
      } else if (gate.type == SkcdGateType::INV) {
        // "replace INV by XOR(a, __dummy1)=not a"
        GT_[q_] = SkcdGateType::XOR;

        unsigned int random_choice =
            pool1[absl::Uniform<uint32_t>(bitgen, 0, pool1.size())];
        B_[q_] = random_choice;
      }
    }

    q_++;
  }

  O_.reserve(outputs_vect.size());

  for (auto &output : outputs_vect) {
    O_.push_back(GetLabel(output));
  }

  // CHECK: the various vectors were NOT resized
  // NOTE: resizing is a performance concern; but it WOULD still work so this is
  // just asserts not throws
  assert(A_.size() == size_to_reserve && "BlifParser: A was resized!");
  assert(B_.size() == size_to_reserve && "BlifParser: B was resized!");
  assert(GO_.size() == size_to_reserve && "BlifParser: GO was resized!");
  assert(GT_.size() == size_to_reserve && "BlifParser: GT was resized!");
  assert(O_.size() == outputs_vect.size() && "BlifParser: O was resized!");

  // INPUTS
  // For now we arbitrarily decide that all inputs are "evaluator_inputs"
  // NOTE: This is only applicable to "generic" circuits(eg adder etc), and
  // there no way in the .blif file format to make it work with Two Party
  // Computation...
  config_.evaluator_inputs.emplace_back(
      EvaluatorInputsType::EVALUATOR_INPUTS_RND, n_);
  assert(config_.evaluator_inputs.size() && "BlifParser: inputs not set!");
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
  if (labels_map_.size() > std::numeric_limits<uint32_t>::max()) {
    throw std::runtime_error("BlifParser: label does not fit on 32 bits!");
  }
  return labels_map_.size() - 1;
}

void BlifParser::ReplaceConfig(const SkcdConfig &other) { config_ = other; }

}  // namespace interstellar