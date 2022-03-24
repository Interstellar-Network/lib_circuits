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

#include <string>

#include "drawable/drawable.h"

namespace interstellar {

/**
 * Helper class that is the step [1] in circuit gen pipeline
 *
 * Allows to:
 * - generate a "bitmap" of the desired size/resolution; then converts it to a
 * .v circuit
 * - generate the define.v containing the Verilog macro used by all
 * xorexpand/rndswitch/display-main
 *
 * NOTE: b/c this class DOES NOT use Verilog/ABC, all is done in buffers(eg
 * string) instead of using files
 */
class Segments2Pixels {
 public:
  /**
   * IMPORTANT When constructing the "full bitmap" here we COULD easily
   * draw different kind of digits side-by-side(eg a 7 segments next to a 14
   * segments)
   * But we CHECK and enforce that all the Drawable are the same class b/c
   * without this we would need to complexify the metadata passed around from
   * here all the way to the Packmsg.
   * To generate the correct OTP/permutation in the Packmsg we MUST make sure
   * what we draw here matches the Packmsg.
   */
  // TODO Span for "drawables"
  Segments2Pixels(uint32_t width, uint32_t height,
                  const std::vector<drawable::Drawable>& drawables);

  // Disable copy semantics.
  Segments2Pixels(const Segments2Pixels&) = delete;
  Segments2Pixels& operator=(const Segments2Pixels&) = delete;

  /**
   * Return: buffer containing a valid Verilog .v circuit
   *
   * It is used to compile the final circuit "display-main.v"
   */
  std::string GenerateVerilog() const;

  std::string GetDefines() const;
  const absl::flat_hash_map<std::string, uint32_t>& GetConfig() const;

 private:
  uint32_t width_ = 0, height_ = 0, nb_segments_ = 0;
  const std::vector<drawable::Drawable>& drawables_;

  absl::flat_hash_map<std::string, uint32_t> config_;
};

}  // namespace interstellar
