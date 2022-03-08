#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <glog/logging.h>

#include "circuit_lib.h"

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

  interstellar::CircuitPipeline::GenerateSkcd(output_skcd_path,
                                              {input_verilog_path});

  return 0;
}
