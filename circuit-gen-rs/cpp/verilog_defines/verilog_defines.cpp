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

#include "verilog_defines.h"

#include <absl/strings/match.h>
#include <absl/strings/str_join.h>

namespace interstellar {

namespace verilog {

Defines::Defines() {}

void Defines::AddDefine(std::string_view key, uint32_t value) {
  AddDefine(key, absl::StrCat(value));
}

void Defines::AddDefine(std::string_view key, std::string_view value) {
  if (absl::StartsWith(key, "`")) {
    throw std::runtime_error("key MUST NOT start with a backtick(`)!");
  }

  defines_map_.try_emplace(key, std::make_optional(value));
  assert(defines_map_.at(key) == value && "new value was not updated!");
}

void Defines::AddDefine(std::string_view key) {
  if (absl::StartsWith(key, "`")) {
    throw std::runtime_error("key MUST NOT start with a backtick(`)!");
  }

  defines_map_.try_emplace(key, std::nullopt);
  assert(defines_map_.at(key) == std::nullopt && "new value was not updated!");
}

std::string Defines::GetDefinesVerilog() {
  std::string defines_buf;
  for (auto const& [key, val] : defines_map_) {
    if (val) {
      absl::StrAppend(&defines_buf, "`define ", key, " ", *val, "\n");
    } else {
      absl::StrAppend(&defines_buf, "`define ", key, "\n");
    }
  }

  return defines_buf;
}

}  // namespace verilog

}  // namespace interstellar
