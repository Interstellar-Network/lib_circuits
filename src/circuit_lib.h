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

#include <boost/filesystem.hpp>

namespace interstellar {

namespace CircuitPipeline {

/**
 * GenerateSkcd: variant that writes the .skcd to the target path
 */
void GenerateSkcd(boost::filesystem::path skcd_output_path,
                  const std::vector<std::string_view> &verilog_inputs_paths);

/**
 * GenerateSkcd: variant that return a .skcd protobuf buffer
 */
std::string GenerateSkcd(
    const std::vector<std::string_view> &verilog_inputs_paths);

/**
 * Specialization of GenerateSkcd for our own "circuit-display.v"
 */
void GenerateDisplaySkcd(boost::filesystem::path skcd_output_path,
                         u_int32_t width, u_int32_t height);

/**
 * Variant of "GenerateDisplaySkcd" that returns a buffer instead of writing to
 * a file.
 */
std::string GenerateDisplaySkcd(u_int32_t width, u_int32_t height);

}  // namespace CircuitPipeline

}  // namespace interstellar