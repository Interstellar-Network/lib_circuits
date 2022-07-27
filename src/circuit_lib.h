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
#include <string>
#include <unordered_map>

namespace interstellar {

namespace circuits {

/**
 * GenerateSkcd: variant that writes the .skcd to the target path
 */
void GenerateSkcd(boost::filesystem::path skcd_output_path,
                  const std::vector<std::string_view> &verilog_inputs_paths);

/**
 * GenerateSkcd: variant that return a .skcd protobuf buffer
 * IMPORTANT: used by api_circuits
 */
std::string GenerateSkcd(
    const std::vector<std::string_view> &verilog_inputs_paths);

/**
 * See also DigitSegmentsType; but that is more a "type" vs implementation
 * thing. eg we would have two separate implementation that are the same
 * DigitSegmentsType::seven_segs using different png/different font/etc
 * So it is better to use a separate enum
 */
enum class DisplayDigitType { seven_segments_png };

/**
 * Specialization of GenerateSkcd for our own "circuit-display.v"
 *
 * @param digits_bboxes vector of ~drawable::RelativeBBox passed as pair<pair>
 * eg digits_bboxes =
 * RelativeBBox(Point2DRelative(0.25f, 0.1f), Point2DRelative(0.45f, 0.9f))
 * passed as {{0.25f, 0.1f},{0.45f, 0.9f}}
 * This is done in order to avoid exposing
 * #include "drawable/drawable.h" publicly
 */
void GenerateDisplaySkcd(
    boost::filesystem::path skcd_output_path, u_int32_t width, u_int32_t height,
    DisplayDigitType digit_type,
    std::vector<std::tuple<float, float, float, float>> &&digits_bboxes,
    std::unordered_map<std::string, uint32_t> *skcd_config);

/**
 * Variant of "GenerateDisplaySkcd" that returns a buffer instead of writing to
 * a file.
 *
 * example of setup for a "2 digits OTP message" vs "pinpad":

  std::vector<std::tuple<float, float, float, float>> digits_bboxes;

  // TODO proper offset,margin,etc
  if (is_message) {
    if (nb_digits == 2) {
      digits_bboxes.emplace_back(0.25f, 0.1f, 0.45f, 0.9f);
      digits_bboxes.emplace_back(0.55f, 0.1f, 0.75f, 0.9f);
    } else {
      throw std::runtime_error("NotImplemented nb_digits == " +
                               std::to_string(nb_digits));
    }
  } else {
    for (int i = 0; i < 10; i++) {
      digits_bboxes.emplace_back(0.1f * i, 0.0f, 0.1f * (i + 1), 1.0f);
    }
  }

 * You can obvviously make the number of digits dynamic, adjust the offset,
 * margin, etc
 */
std::string GenerateDisplaySkcd(
    u_int32_t width, u_int32_t height, DisplayDigitType digit_type,
    std::vector<std::tuple<float, float, float, float>> &&digits_bboxes,
    std::unordered_map<std::string, uint32_t> *skcd_config);

}  // namespace circuits

}  // namespace interstellar