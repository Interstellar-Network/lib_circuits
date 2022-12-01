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

#include <absl/strings/str_cat.h>
#include <absl/strings/str_split.h>
#include <fmt/format.h>

#include <cassert>
#include <iostream>
#include <stack>

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
SkcdGate ParseGateLine(std::string_view gate_line) {
  std::vector<std::string_view> gate_vect =
      absl::StrSplit(gate_line, ' ', absl::SkipEmpty());

  if (gate_vect[0] != ".gate") {
    throw std::runtime_error("BlifParser: NOT a .gate line!");
  }

  SkcdGate gate;

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
void BlifParser::ParseBuffer(std::string_view blif_buffer) {
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

          lines.emplace_back(*line_with_continuation);

          // reset the line_with_continuation buffer
          continuation_buffers.push(std::make_unique<std::string>());
          line_with_continuation = continuation_buffers.top().get();
        } else {
          // Standard line
          lines.emplace_back(line);
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

  // Step 1: parse all the gates
  // NOTE: excluding the last one, which is ".endmodule"
  gates_.reserve(lines.size());  // prealloc; slightly too many b/c we have
                                 // already some of those lines
  for (auto line_gate = lines.begin() + 3; line_gate != lines.end() - 1;
       ++line_gate) {
    SkcdGate gate = ParseGateLine(*line_gate);
    gates_.emplace_back(gate);
  }

  // Now for the outputs
  O_.reserve(outputs_vect.size());
  for (auto &output : outputs_vect) {
    O_.emplace_back(output);
  }

  // INPUTS
  // For now we arbitrarily decide that all inputs are "evaluator_inputs"
  // NOTE: This is only applicable to "generic" circuits(eg adder etc), and
  // there no way in the .blif file format to make it work with Two Party
  // Computation...
  config_.evaluator_inputs.emplace_back(
      EvaluatorInputsType::EVALUATOR_INPUTS_RND, inputs_vect.size());
  assert(config_.evaluator_inputs.size() && "BlifParser: inputs not set!");
}

void BlifParser::ReplaceConfig(const SkcdConfig &other) { config_ = other; }

}  // namespace interstellar