#include <absl/flags/flag.h>
#include <absl/flags/parse.h>

#include "circuit_lib.h"

ABSL_FLAG(std::string, output_skcd_path, "output.skcd.pb.bin",
          "output file dir");

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  auto output_skcd_path = absl::GetFlag(FLAGS_output_skcd_path);

  interstellar::CircuitPipeline::GenerateSkcd(output_skcd_path);

  return 0;
}
