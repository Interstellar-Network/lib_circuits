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
