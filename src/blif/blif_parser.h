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

#pragma once

#include <cmath>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "gate_types.h"
#include "skcd_config.h"

namespace interstellar {

class Label {
 public:
  std::string_view str_v;
};

/**
 * Basic .blif parser.
 * Used to parse the blif juste before conversion to .skcd
 *
 * As of the latest refactor (for Swanky/Rust, late 2022) .skcd is now
 * a basic raw mapping of the .blif format.
 * IE no more gate rewriting (to XOR), no label recomputation via Hashmap, etc
 *
 * WARNING: only parses a subset of .blif !
 */
class BlifParser {
 public:
  BlifParser();

  // Disable copy semantics.
  BlifParser(const BlifParser &) = delete;
  BlifParser &operator=(const BlifParser &) = delete;
  // noncopyable, but we still need the moves
  BlifParser(BlifParser &&) = default;
  BlifParser &operator=(BlifParser &&) = default;

  // TODO handle the '-z' option, see lib_python
  // TODO return a struct/class
  // TODO string_view
  void ParseBuffer(std::string_view blif_buffer);

  // WARNING: the functions below return REF on the internal vectors;
  // They are used when constructing the Skcd, by move so ref is fine.

  const std::vector<SkcdGate> &GetGates() const { return gates_; }

  const std::vector<std::string> &GetOutputs() const { return outputs_; }

  const std::vector<std::string> &GetInputs() const { return inputs_; }

  const SkcdConfig &GetConfig() const { return config_; }

  void ReplaceConfig(const SkcdConfig &other);

 private:
  std::vector<SkcdGate> gates_;
  std::vector<std::string> outputs_;  // outputs
  std::vector<std::string> inputs_;   // inputs

  SkcdConfig config_;
};

}  // namespace interstellar