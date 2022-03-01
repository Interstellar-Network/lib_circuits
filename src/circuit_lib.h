#pragma once

#include <boost/filesystem.hpp>

namespace interstellar {

namespace CircuitPipeline {

void GenerateSkcd(boost::filesystem::path skcd_output_path,
                  const std::vector<std::string_view> &verilog_inputs_paths);

/**
 * Specialization of GenerateSkcd for our own "circuit-display.v"
 */
void GenerateDisplaySkcd(boost::filesystem::path skcd_output_path,
                         u_int32_t width, u_int32_t height);

}  // namespace CircuitPipeline

}  // namespace interstellar