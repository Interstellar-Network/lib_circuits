#include <absl/flags/flag.h>
#include <absl/flags/parse.h>

#include "circuit_lib.h"

ABSL_FLAG(std::string, output_skcd_path, "output.skcd.pb.bin",
          "output file dir");
ABSL_FLAG(u_int32_t, width, 1280 / 2, "width");
ABSL_FLAG(u_int32_t, height, 720 / 2, "height");

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  auto output_skcd_path = absl::GetFlag(FLAGS_output_skcd_path);
  auto width = absl::GetFlag(FLAGS_width);
  auto height = absl::GetFlag(FLAGS_height);

  interstellar::CircuitPipeline::GenerateSkcd(output_skcd_path, width, height);

  return 0;
}
