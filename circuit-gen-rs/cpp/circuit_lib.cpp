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

#include <absl/random/random.h>
#include <absl/strings/str_cat.h>

#include "abc_wrapper/abc_wrapper.h"
#include "drawable/drawable.h"
#include "drawable/drawable_digit_png.h"
#include "resources.h"
#include "segments2pixels/segments2pixels.h"
#include "utils/files/utils_files.h"
#include "verilog/verilog_compiler.h"

namespace {

std::string GenerateBlifBlif(
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

  auto tmp_blif_blif_str = interstellar::utils::ReadFile(final_blif_blif_path);
  return tmp_blif_blif_str;
}

/**
 * IMPORTANT the "what to draw" MUST be kept alive for the whole duration of
 * Segments2Pixels!
 * For now we use a static b/c that way we also avoid re-instantiating during
 * each circuit gen. Typically those are heavy to instantiate(eg parsing a png,
 * etc)
 */
const interstellar::drawable::IDrawableSegmentedDigitRelCoordsLocal &
GetDrawableFromDigitType(interstellar::circuits::DisplayDigitType digit_type) {
  switch (digit_type) {
    case interstellar::circuits::DisplayDigitType::seven_segments_png:
      // TOREMOVE this SHOULD not work, it MUST be kept alive for
      // Segments2Pixels
      static interstellar::drawable::DrawableDigitPng seven_segs_png;
      return seven_segs_png;
      break;
  }

  // GCC: "error: control reaches end of non-void function
  // [-Werror=return-type]"
  __builtin_unreachable();
}

}  // namespace

namespace interstellar {

namespace circuits {

// TODO how to handle InitGoogleLogging ?

/**
 * IMPORTANT: used by api_circuits
 */
std::string GenerateSkcd(std::string_view verilog_buf) {
  auto tmp_dir = utils::TempDir();

  // write the buf to a temp file
  auto temp_v_path = tmp_dir.GetPath() / "temp.v";
  utils::WriteToFile(temp_v_path, verilog_buf);

  auto final_blif_blif_str =
      GenerateBlifBlif({temp_v_path.generic_string()}, tmp_dir);

  return final_blif_blif_str;
}

std::string GenerateDisplaySkcd(
    u_int32_t width, u_int32_t height, DisplayDigitType digit_type,
    bool has_watermark,
    std::vector<std::tuple<float, float, float, float>> &&digits_bboxes,
    uint32_t *nb_segments, uint32_t *rndsize) {
  auto tmp_dir = utils::TempDir();

  const auto &what_to_draw = GetDrawableFromDigitType(digit_type);
  std::vector<drawable::Drawable<drawable::RelativeBBox>> drawables;
  for (auto &&digit_bbox : digits_bboxes) {
    drawables.emplace_back(
        drawable::RelativeBBox(std::get<0>(digit_bbox), std::get<1>(digit_bbox),
                               std::get<2>(digit_bbox),
                               std::get<3>(digit_bbox)),
        what_to_draw);
  }

  // [1] generate Verilog segments2pixels.v
  auto segments2pixels =
      Segments2Pixels(width, height, drawables, has_watermark);
  auto segments2pixels_v_str = segments2pixels.GenerateVerilog();

  // write this to segments2pixels.v (in the temp dir)
  // because Yosys only handles files, not buffers
  auto segments2pixels_v_path = tmp_dir.GetPath() / "segments2pixels.v";
  utils::WriteToFile(segments2pixels_v_path, segments2pixels_v_str);

  auto defines_v_str = segments2pixels.GetDefines();
  // write this to defines.v (in the temp dir)
  // because Yosys only handles files, not buffers
  auto defines_v_path = tmp_dir.GetPath() / "defines.v";
  utils::WriteToFile(defines_v_path, defines_v_str);

  auto final_blif_blif_str = GenerateBlifBlif(
      {
          defines_v_path.generic_string(),
          segments2pixels_v_path.generic_string(),
          // new circuit module to display segments with variable probabilities instead of 1/2
          absl::StrCat(interstellar::data_dir, "/verilog/LFSR_comb.v"),
          absl::StrCat(interstellar::data_dir, "/verilog/rndswitch.v"),
          absl::StrCat(interstellar::data_dir, "/verilog/watermark.v"),
          absl::StrCat(interstellar::data_dir, "/verilog/display-main.v"),
      },  
        
      tmp_dir);

  segments2pixels.GetConfig(nb_segments, rndsize);

  return final_blif_blif_str;
}

}  // namespace circuits

}  // namespace interstellar