#pragma once

#include <string_view>
#include <vector>

namespace interstellar {

/**
 * Helper class to compile a .v into a .blif
 */
namespace VerilogHelper {

/**
 * .v -> .blif; with yosys
 *
 * IMPORTANT: Yosys:: as no concept of exception or error code, it will simply
 * exit/abort in case of error
 *
 * @param inputs_v_full_paths paths to the (several) Verilog .v to process
 * @param output_blif_full_path path to the output .blif
 */
void CompileVerilog(const std::vector<std::string_view> &inputs_v_full_paths,
                    std::string_view output_blif_full_path);

}  // namespace VerilogHelper

}  // namespace interstellar
