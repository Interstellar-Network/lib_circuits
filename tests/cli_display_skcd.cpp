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

ABSL_FLAG(std::string, output_skcd_path, "output.skcd.pb.bin",
          "output file dir");
ABSL_FLAG(u_int32_t, width, 1280 / 2, "width");
ABSL_FLAG(u_int32_t, height, 720 / 2, "height");

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);

  auto output_skcd_path = absl::GetFlag(FLAGS_output_skcd_path);
  auto width = absl::GetFlag(FLAGS_width);
  auto height = absl::GetFlag(FLAGS_height);

  interstellar::CircuitPipeline::GenerateDisplaySkcd(output_skcd_path, width,
                                                     height);

  return 0;
}
