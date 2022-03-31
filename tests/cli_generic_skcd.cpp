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
          "output file .skcd");
ABSL_FLAG(std::string, input_verilog_path, "", "input file .v");

/**
 * Compile a circuit (.skcd) for a generic Verilog given as input
 * eg adder/sha/aes/etc
 */
int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  absl::ParseCommandLine(argc, argv);

  auto output_skcd_path = absl::GetFlag(FLAGS_output_skcd_path);
  auto input_verilog_path = absl::GetFlag(FLAGS_input_verilog_path);

  if (input_verilog_path.empty()) {
    throw std::invalid_argument("required: --input_verilog_path=path/to.v");
  }

  circuits::GenerateSkcd(output_skcd_path, {input_verilog_path});

  return 0;
}
