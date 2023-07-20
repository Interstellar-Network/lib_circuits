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

#include "rust_wrapper.h"

#include <functional>

#include "circuit_lib.h"

// generated
// needed only if shared structs
#include "circuit-gen-rs/src/lib.rs.h"

rust::Vec<u_int8_t> GenerateDisplayCircuit(
    uint32_t width, uint32_t height, const rust::Vec<float> &digits_bboxes,
    ConfigHelper &config_helper) {
  // CHECK: digits_bboxes SHOULD be a list ob bboxes, passed as (x1,y1,x2,y2)
  size_t digits_bboxes_size = digits_bboxes.size();
  if (!digits_bboxes_size % 4) {
    throw std::invalid_argument(
        "GenerateDisplaySkcd: digits_bboxes must be a list of bboxes(ie size "
        "== mod 4)");
  }
  std::vector<std::tuple<float, float, float, float>> digits_bboxes_copy;
  digits_bboxes_copy.reserve(digits_bboxes_size / 4);
  for (uint32_t i = 0; i < digits_bboxes_size; i += 4) {
    digits_bboxes_copy.emplace_back(digits_bboxes[i], digits_bboxes[i + 1],
                                    digits_bboxes[i + 2], digits_bboxes[i + 3]);
  }

  // bool has_watermark: hardcoded to "true" for now b/c it was just for testing
  // purposes
  //  It is only useful if "Free-XOR" opt is implemented.
  //  Also we will probabably completely change the circuits/Verilog in the next
  //  milestone.
  bool has_watermark = true;
  auto final_blif_blif_str = interstellar::circuits::GenerateDisplaySkcd(
      width, height,
      interstellar::circuits::DisplayDigitType::seven_segments_png,
      has_watermark, std::move(digits_bboxes_copy), &config_helper.nb_segments,
      &config_helper.rndsize);

  // copy to Rust CXX Vec
  rust::Vec<u_int8_t> vec;
  std::copy(final_blif_blif_str.begin(), final_blif_blif_str.end(),
            std::back_inserter(vec));

  config_helper.has_watermark = has_watermark;
  config_helper.width = width;
  config_helper.height = height;

  return vec;
}

rust::Vec<u_int8_t> GenerateGenericCircuit(rust::Vec<u_int8_t> verilog_buf) {
  std::string buf;
  std::copy(verilog_buf.begin(), verilog_buf.end(), std::back_inserter(buf));

  auto buf_str = interstellar::circuits::GenerateSkcd(buf);

  // copy to Rust CXX Vec
  rust::Vec<u_int8_t> vec;
  std::copy(buf_str.begin(), buf_str.end(), std::back_inserter(vec));
  return vec;
}
