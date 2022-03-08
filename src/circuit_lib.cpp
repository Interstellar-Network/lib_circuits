#include "circuit_lib.h"

#include <absl/strings/str_cat.h>

#include "abc_wrapper/abc_wrapper.h"
#include "blif/blif_parser.h"
#include "resources.h"
#include "segments2pixels/segments2pixels.h"
#include "skcd/skcd.h"
#include "utils/files/utils_files.h"
#include "verilog/verilog_compiler.h"

namespace {

interstellar::BlifParser GenerateBlifBlif(
    const std::vector<std::string_view> &verilog_inputs_paths,
    const interstellar::utils::TempDir &tmp_dir) {
  auto output_blif_path = tmp_dir.GetPath() / "output.blif";

  interstellar::VerilogHelper::CompileVerilog(
      verilog_inputs_paths, output_blif_path.generic_string());

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
  auto blif_parser = interstellar::BlifParser();
  auto tmp_blif_blif_str = interstellar::utils::ReadFile(final_blif_blif_path);
  blif_parser.ParseBuffer(tmp_blif_blif_str, true);

  return blif_parser;
}

}  // namespace

namespace interstellar {

namespace CircuitPipeline {

// TODO how to handle InitGoogleLogging ?

void GenerateSkcd(boost::filesystem::path skcd_output_path,
                  const std::vector<std::string_view> &verilog_inputs_paths,
                  const utils::TempDir &tmp_dir) {
  auto blif_parser = GenerateBlifBlif(verilog_inputs_paths, tmp_dir);

  interstellar::skcd::WriteToFile(skcd_output_path, blif_parser);
}

std::string GenerateSkcd(
    const std::vector<std::string_view> &verilog_inputs_paths,
    const utils::TempDir &tmp_dir) {
  auto blif_parser = GenerateBlifBlif(verilog_inputs_paths, tmp_dir);

  return interstellar::skcd::Serialize(blif_parser);
}

/**
 * [internal]
 * Overload[1] to call the main "GenerateSkcd" reusing a given TempDir
 */
void GenerateSkcd(boost::filesystem::path skcd_output_path,
                  const std::vector<std::string_view> &verilog_inputs_paths) {
  auto tmp_dir = utils::TempDir();
  GenerateSkcd(skcd_output_path, verilog_inputs_paths, tmp_dir);
}

/**
 * [internal]
 * Overload[2] to call the main "GenerateSkcd" reusing a given TempDir
 */
std::string GenerateSkcd(
    const std::vector<std::string_view> &verilog_inputs_paths) {
  auto tmp_dir = utils::TempDir();
  return GenerateSkcd(verilog_inputs_paths, tmp_dir);
}

void GenerateDisplaySkcd(boost::filesystem::path skcd_output_path,
                         u_int32_t width, u_int32_t height) {
  auto result_skcd_buf = GenerateDisplaySkcd(width, height);

  utils::WriteToFile(skcd_output_path, result_skcd_buf);
}

std::string GenerateDisplaySkcd(u_int32_t width, u_int32_t height) {
  auto tmp_dir = utils::TempDir();

  // [1] generate Verilog segments2pixels.v
  auto segment2pixels = Segments2Pixels(width, height);
  auto segment2pixels_v_str = segment2pixels.GenerateVerilog();

  // write this to segments2pixels.v (in the temp dir)
  // because Yosys only handles files, not buffers
  auto segments2pixels_v_path = tmp_dir.GetPath() / "segments2pixels.v";
  utils::WriteToFile(segments2pixels_v_path, segment2pixels_v_str);

  auto defines_v_str = segment2pixels.GetDefines();
  // write this to defines.v (in the temp dir)
  // because Yosys only handles files, not buffers
  auto defines_v_path = tmp_dir.GetPath() / "defines.v";
  utils::WriteToFile(defines_v_path, defines_v_str);

  std::string result_skcd_buf = GenerateSkcd({
      defines_v_path.generic_string(),
      segments2pixels_v_path.generic_string(),
      absl::StrCat(interstellar::data_dir, "/verilog/rndswitch.v"),
      absl::StrCat(interstellar::data_dir, "/verilog/xorexpand.v"),
      absl::StrCat(interstellar::data_dir, "/verilog/display-main.v"),
  });

  return result_skcd_buf;
}

}  // namespace CircuitPipeline

}  // namespace interstellar