#pragma once

#include <string_view>

namespace interstellar {

/**
 * Helper class to compile a .v into a .blif
 */
namespace YosysHelper {

/**
 * .v -> .blif; with yosys
 *
 * IMPORTANT: Yosys:: as no concept of exception or error code, it will simply
 * exit/abort in case of error
 *
 * @param input_v_full_path path to the .v to process
 * @param output_blif_full_path path to the output .blif
 */
void CompileVerilog(std::string_view input_v_full_path,
                    std::string_view output_blif_full_path);

}  // namespace YosysHelper

}  // namespace interstellar
