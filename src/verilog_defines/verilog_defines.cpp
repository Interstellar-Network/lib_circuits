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

  _defines_map.try_emplace(key, value);
  assert(_defines_map.at(key) == value && "new value was not updated!");
}

std::string Defines::GetDefinesVerilog() {
  std::string defines_buf;
  for (auto const& [key, val] : _defines_map) {
    absl::StrAppend(&defines_buf, "`define ", key, " ", val, "\n");
  }

  return defines_buf;
}

}  // namespace verilog

}  // namespace interstellar
