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
#include <vector>

namespace interstellar {

namespace drawable {

// TODO internal to the class?
struct ColorRGBA {
  u_int8_t r, g, b, a;

  ColorRGBA(u_int8_t r, u_int8_t g, u_int8_t b, u_int8_t a);

  /**
   * Construct from an HEX string (with or without prefix #)
   * Trailing A is omitted if not given.
   */
  ColorRGBA(std::string_view hex_string);

  // Make this class hashable to be used as key by absl::flat_hash_map
  template <typename H>
  friend H AbslHashValue(H h, const ColorRGBA& c) {
    return H::combine(std::move(h), c.r, c.g, c.b, c.a);
  }

  friend bool operator==(const ColorRGBA& lhs, const ColorRGBA& rhs) {
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
  }
};

/**
 * Just convert kMapColorToSegId in the .cpp into map ColorRGBA->uint32_t(ie
 * SegmentID)
 */
absl::flat_hash_map<ColorRGBA, u_int32_t> GetMapColorToSegId();

}  // namespace drawable

}  // namespace interstellar