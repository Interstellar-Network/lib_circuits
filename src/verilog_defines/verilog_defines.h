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
#include <absl/container/flat_hash_map.h>

#include <initializer_list>
#include <string>

namespace interstellar {

namespace verilog {

/**
 * Helper class to represent `define macros in Verilog
 */
class Defines {
 public:
  Defines();

  /**
   * Verilog requires its "defines" variable to be prefixed with a backtick:
   * eg "assign ... `SOMEDEF" only when USED.
   * The definition itself MUST NOT have a backtick.
   */
  void AddDefine(std::string_view key, std::string_view value);

  void AddDefine(std::string_view key, uint32_t value);

  /**
   * Return the defines properly formatted as Verilog
   * eg:
        `define WIDTH 56
        `define HEIGHT 24
        `define RNDSIZE 9
        `define BITMAP_NB_SEGMENTS 28
   */
  std::string GetDefinesVerilog();

 private:
  absl::flat_hash_map<std::string, std::string> _defines_map;
};

}  // namespace verilog

}  // namespace interstellar
