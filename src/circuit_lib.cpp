#include "circuit_lib.h"

#include "segments2pixels/segments2pixels.h"
#include "utils/files/utils_files.h"
#include "verilog/verilog_compiler.h"

namespace interstellar {

namespace CircuitPipeline {

// TODO how to handle InitGoogleLogging ?

Skcd GenerateSkcd() {
  // [1] generate Verilog segments2pixels.v
  auto segment2pixels = Segments2Pixels(42, 42);
  auto segment2pixels_v_str = segment2pixels.GenerateVerilog();

  auto tmp_dir = utils::TempDir();

  // write this to segments2pixels.v (in the temp dir)
  // because Yosys only handles files, not buffers
  auto segments2pixels_v_path = tmp_dir.GetPath() / "segments2pixels.v";
  utils::WriteToFile(segments2pixels_v_path, segment2pixels_v_str);

  // [?]
  VerilogHelper::CompileVerilog({segments2pixels_v_path.generic_string()},
                                "output.blif");

  // convert .blif.blif -> .skcd
  auto blif_parser = BlifParser();
  blif_parser.ParseBuffer(segment2pixels_v_str, true);

  // [?]
  return Skcd(std::move(blif_parser));
}

}  // namespace CircuitPipeline

}  // namespace interstellar