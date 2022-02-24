#include "circuit_lib.h"

#include <absl/strings/str_cat.h>

#include "abc_wrapper/abc_wrapper.h"
#include "blif/blif_parser.h"
#include "resources.h"
#include "segments2pixels/segments2pixels.h"
#include "skcd/skcd.h"
#include "utils/files/utils_files.h"
#include "verilog/verilog_compiler.h"

namespace interstellar {

namespace CircuitPipeline {

// TODO how to handle InitGoogleLogging ?

void GenerateSkcd(boost::filesystem::path skcd_output_path, u_int32_t width,
                  u_int32_t height) {
  // [1] generate Verilog segments2pixels.v
  auto segment2pixels = Segments2Pixels(width, height);
  auto segment2pixels_v_str = segment2pixels.GenerateVerilog();

  auto tmp_dir = utils::TempDir();

  // write this to segments2pixels.v (in the temp dir)
  // because Yosys only handles files, not buffers
  auto segments2pixels_v_path = tmp_dir.GetPath() / "segments2pixels.v";
  utils::WriteToFile(segments2pixels_v_path, segment2pixels_v_str);

  // [?]
  auto output_blif_path = tmp_dir.GetPath() / "output.blif";
  VerilogHelper::CompileVerilog(
      {
          // TODO define.v: generate dynamically and write to file(ideally
          // generate using a ref to Segments2Pixels)
          absl::StrCat(interstellar::data_dir,
                       "/verilog/generated/message-define.v"),
          segments2pixels_v_path.generic_string(),
          absl::StrCat(interstellar::data_dir, "/verilog/rndswitch.v"),
          absl::StrCat(interstellar::data_dir, "/verilog/xorexpand.v"),
          absl::StrCat(interstellar::data_dir, "/verilog/display-main.v"),
      },
      output_blif_path.generic_string());

  // [?] abc pass: .blif containing multiple models
  // -> .blif.blif with "main" only
  //
  // append NOT join else the final path is eg
  // "/tmp/interstellar-circuits/XXX/output.blif/.blif"
  auto final_blif_blif_path = output_blif_path.generic_string() + ".blif";
  interstellar::ABC::Run(output_blif_path.generic_string(),
                         final_blif_blif_path);

  // convert .blif.blif -> ~.skcd
  // NOTE: Skcd class was previously used to pass the data around, but it has
  // been replaced by protobuf serialization
  auto blif_parser = BlifParser();
  auto tmp_blif_blif_str = utils::ReadFile(final_blif_blif_path);
  blif_parser.ParseBuffer(tmp_blif_blif_str, true);

  // [?]
  interstellar::skcd::WriteToFile(skcd_output_path, blif_parser);
}

}  // namespace CircuitPipeline

}  // namespace interstellar