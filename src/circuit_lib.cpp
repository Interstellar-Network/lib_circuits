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

#include "circuit_lib.h"

#include <absl/strings/str_cat.h>

#include "abc_wrapper/abc_wrapper.h"
#include "blif/blif_parser.h"
#include "drawable/drawable_digit_png.h"
#include "resources.h"
#include "segments2pixels/segments2pixels.h"
#include "skcd/skcd.h"
#include "utils/files/utils_files.h"
#include "verilog/verilog_compiler.h"

namespace {

interstellar::BlifParser GenerateBlifBlif(
    const std::vector<std::string_view> &verilog_inputs_paths,
    const interstellar::utils::TempDir &tmp_dir,
    absl::flat_hash_map<std::string, uint32_t> &&config) {
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
  auto blif_parser = interstellar::BlifParser(std::move(config));
  auto tmp_blif_blif_str = interstellar::utils::ReadFile(final_blif_blif_path);
  blif_parser.ParseBuffer(tmp_blif_blif_str, true);

  return blif_parser;
}

/**
 * overload GenerateBlifBlif with an empty config
 */
interstellar::BlifParser GenerateBlifBlif(
    const std::vector<std::string_view> &verilog_inputs_paths,
    const interstellar::utils::TempDir &tmp_dir) {
  absl::flat_hash_map<std::string, uint32_t> config;
  return GenerateBlifBlif(verilog_inputs_paths, tmp_dir, std::move(config));
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
    const utils::TempDir &tmp_dir,
    absl::flat_hash_map<std::string, uint32_t> &&config) {
  auto blif_parser =
      GenerateBlifBlif(verilog_inputs_paths, tmp_dir, std::move(config));

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
    const std::vector<std::string_view> &verilog_inputs_paths,
    absl::flat_hash_map<std::string, uint32_t> &&config) {
  auto tmp_dir = utils::TempDir();
  return GenerateSkcd(verilog_inputs_paths, tmp_dir, std::move(config));
}

void GenerateDisplaySkcd(boost::filesystem::path skcd_output_path,
                         u_int32_t width, u_int32_t height, uint32_t nb_digits,
                         bool is_message) {
  auto result_skcd_buf =
      GenerateDisplaySkcd(width, height, nb_digits, is_message);

  utils::WriteToFile(skcd_output_path, result_skcd_buf);
}

std::string GenerateDisplaySkcd(u_int32_t width, u_int32_t height,
                                uint32_t nb_digits, bool is_message) {
  auto tmp_dir = utils::TempDir();

  // [1] generate Verilog segments2pixels.v
  // TODO get that from a static? or better to let the caller do that?
  drawable::DrawableDigitPng seven_segs_png;
  std::vector<drawable::Drawable> drawables;

  // TODO proper offset,margin,etc
  if (is_message) {
    switch (nb_digits) {
      case 0:
        // 'pin_only' transactions: no OTP on the messge
        throw std::logic_error("GenerateDisplaySkcd NO OTP UnimplementedError");
        break;

      case 2:
        drawables.emplace_back(std::make_unique<drawable::RelativeBBox>(
                                   drawable::Point2DRelative(0.25f, 0.1f),
                                   drawable::Point2DRelative(0.45f, 0.9f)),
                               seven_segs_png);
        drawables.emplace_back(std::make_unique<drawable::RelativeBBox>(
                                   drawable::Point2DRelative(0.55f, 0.1f),
                                   drawable::Point2DRelative(0.75f, 0.9f)),
                               seven_segs_png);
        break;

      default:
        throw std::runtime_error(
            "GenerateSegmentToPixelVerilog : unsupported msgsize : " +
            std::to_string(nb_digits));
    }

  } else {
    for (int i = 0; i < 10; i++) {
      drawables.emplace_back(
          std::make_unique<drawable::RelativeBBox>(
              drawable::Point2DRelative(0.1f * i, 0.0f),
              drawable::Point2DRelative(0.1f * (i + 1), 1.0f)),
          seven_segs_png);
    }
  }

  auto segments2pixels = Segments2Pixels(width, height, drawables);
  auto segments2pixels_v_str = segments2pixels.GenerateVerilog();
  auto config = segments2pixels.GetConfig();

  // write this to segments2pixels.v (in the temp dir)
  // because Yosys only handles files, not buffers
  auto segments2pixels_v_path = tmp_dir.GetPath() / "segments2pixels.v";
  utils::WriteToFile(segments2pixels_v_path, segments2pixels_v_str);

  auto defines_v_str = segments2pixels.GetDefines();
  // write this to defines.v (in the temp dir)
  // because Yosys only handles files, not buffers
  auto defines_v_path = tmp_dir.GetPath() / "defines.v";
  utils::WriteToFile(defines_v_path, defines_v_str);

  std::string result_skcd_buf = GenerateSkcd(
      {
          defines_v_path.generic_string(),
          segments2pixels_v_path.generic_string(),
          absl::StrCat(interstellar::data_dir, "/verilog/rndswitch.v"),
          absl::StrCat(interstellar::data_dir, "/verilog/xorexpand.v"),
          absl::StrCat(interstellar::data_dir, "/verilog/display-main.v"),
      },
      std::move(config));

  return result_skcd_buf;
}

}  // namespace CircuitPipeline

}  // namespace interstellar