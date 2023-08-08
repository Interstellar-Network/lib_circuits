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

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <glog/logging.h>

#include "circuit_lib.h"

using namespace interstellar;

ABSL_FLAG(std::string, output_skcd_path, "output.skcd.pb.bin",
          "output file dir");
ABSL_FLAG(u_int32_t, width, 1280 / 2, "width");
ABSL_FLAG(u_int32_t, height, 720 / 2, "height");
ABSL_FLAG(u_int32_t, nb_digits, 2, "nb_digits");
ABSL_FLAG(bool, is_message, true, "is_message");
ABSL_FLAG(bool, has_watermark, true, 
          "has_watermark: cf HAS_WATERMARK in data/verilog/");

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);

  auto output_skcd_path = absl::GetFlag(FLAGS_output_skcd_path);
  auto width = absl::GetFlag(FLAGS_width);
  auto height = absl::GetFlag(FLAGS_height);
  auto nb_digits = absl::GetFlag(FLAGS_nb_digits);
  auto is_message = absl::GetFlag(FLAGS_is_message);
  auto has_watermark = absl::GetFlag(FLAGS_has_watermark);

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

  uint32_t nb_segments, rndsize;
  circuits::GenerateDisplaySkcd(
      width, height, circuits::DisplayDigitType::seven_segments_png,
      has_watermark, std::move(digits_bboxes), &nb_segments, &rndsize);

  return 0;
}
