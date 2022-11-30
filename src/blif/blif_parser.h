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
#include <unordered_map>
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

  unsigned int Getn() const { return n_; }

  unsigned int Getm() const { return m_; }

  unsigned int Getq() const { return q_; }

  // WARNING: the functions below return REF on the internal vectors;
  // They are used when constructing the Skcd, by move so ref is fine.

  const std::vector<unsigned int> &GetA() const { return A_; }

  const std::vector<unsigned int> &GetB() const { return B_; }

  const std::vector<unsigned int> &GetGO() const { return GO_; }

  const std::vector<SkcdGateType> &GetGT() const { return GT_; }

  const std::vector<unsigned int> &GetO() const { return O_; }

  const SkcdConfig &GetConfig() const { return config_; }

  void ReplaceConfig(const SkcdConfig &other);

 private:
  unsigned int n_ = 0;  // inputs number
  unsigned int m_ = 0;  // outputs number
  unsigned int q_ = 0;  // gates number

  // Vector of "label index"(except GT)
  std::vector<unsigned int> A_;   // gates first inputs
  std::vector<unsigned int> B_;   // gates second inputs
  std::vector<unsigned int> GO_;  // gates outputs
  std::vector<SkcdGateType> GT_;  // gates types
  std::vector<unsigned int> O_;   // outputs

  // Associate a label(as hashed string/string_view) to a index(0-N)
  // TODO(unordered_map?) replace by array LUT?
  std::unordered_map<uint64_t, size_t> labels_map_;
  // size_t current_label_count_ = 0;

  SkcdConfig config_;

  /**
   * "Unlike other string types, you should pass string_view by value just like
 you would an int or a double because string_view is a small value."
   */
  uint32_t GetLabel(std::string_view lbl);
};

}  // namespace interstellar