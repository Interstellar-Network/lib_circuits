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

#include "segments2pixels.h"

#include <absl/container/flat_hash_set.h>
#include <absl/hash/hash.h>
#include <absl/strings/str_cat.h>
#include <fmt/format.h>
#include <glog/logging.h>

#include <algorithm>
#include <filesystem>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "utils/encode_rle/encode_rle.h"
#include "verilog_defines/verilog_defines.h"

namespace interstellar {

template <typename DrawableWhereT>
Segments2Pixels<DrawableWhereT>::Segments2Pixels(
    uint32_t width, uint32_t height,
    const std::vector<drawable::Drawable<DrawableWhereT>>& drawables,
    bool has_watermark)
    : drawables_(drawables) {
  // CHECK drawables MUST NOT be empty
  // We could return early instead of throwing but generating and then garbling
  // a circuit with no input does not really make sense.
  // Also it has never been tested so we would rather throw.
  if (drawables_.empty()) {
    throw std::logic_error("Segments2Pixels: drawables MUST NOT be empty");
  }

  // CHECK that all Drawable are the same class
  drawable::DigitSegmentsType segments_type = drawables_[0].What().GetType();
  uint32_t nb_segs_per_digit = drawables_[0].What().GetNbSegments();
  u_int32_t nb_segments = 0;
  for (const auto& drawable : drawables_) {
    if (drawable.What().GetType() != segments_type) {
      throw std::logic_error(
          "Segments2Pixels: drawing different digits is not allowed");
    }
    nb_segments += drawable.What().GetNbSegments();
  }

  if (nb_segments != drawables_.size() * nb_segs_per_digit) {
    throw std::logic_error("Segments2Pixels: nb_segments mismatch!");
  }

  // RNDSIZE
  // TODO Check
  // Historically(before the support of variable otp_length), message had
  // RNDSIZE=9, and pinpad RNDSIZE=16
  // math.ceil(0.5 * math.sqrt(8 * otp_length * message_seg + 1) + 1)
    
    auto mask_size = static_cast<unsigned int>(10); // size of the LFSR_comb mask
    auto rndsize2 = static_cast<unsigned int>(nb_segments*mask_size); 

  config_.garbler_inputs.emplace_back(
      GarblerInputs{GarblerInputsType::GARBLER_INPUTS_BUF, 1});
  config_.garbler_inputs.emplace_back(GarblerInputs{
      GarblerInputsType::GARBLER_INPUTS_SEVEN_SEGMENTS, nb_segments});
  if (has_watermark) {
    config_.garbler_inputs.emplace_back(GarblerInputs{
        GarblerInputsType::GARBLER_INPUTS_WATERMARK, width * height});
  }
  config_.evaluator_inputs.emplace_back(
      EvaluatorInputs{EvaluatorInputsType::EVALUATOR_INPUTS_RND, rndsize2 }); 
      printf(" nb_segments: %d, EvalInput: %d\n", nb_segments, rndsize2);

  config_.display_config.width = width;
  config_.display_config.height = height;
  config_.display_config.segments_type = static_cast<uint32_t>(segments_type);

  has_watermark_ = has_watermark;
}

template <typename DrawableWhereT>
std::string Segments2Pixels<DrawableWhereT>::GenerateVerilog() const {
  // Generate the complete bitmap, then compute the SegmentID for each pixel
  // Previously it was done is the ctor then stored in class member but it is
  // only used here so no point in doing that
  auto width = config_.display_config.width;
  auto height = config_.display_config.height;
  unsigned int nb_pixels = width * height;
  std::vector<drawable::SegmentID> bitmap_seg_ids =
      Draw(drawables_, width, height);

  std::vector<utils::RLE_int8_t> bitmap_seg_ids_rle =
      utils::compress_rle(bitmap_seg_ids);

  std::string verilog_buf;

  assert(config_.garbler_inputs[0].type ==
             GarblerInputsType::GARBLER_INPUTS_BUF &&
         "wrong config order in ::Segments2Pixels?");
  assert(config_.garbler_inputs[1].type ==
             GarblerInputsType::GARBLER_INPUTS_SEVEN_SEGMENTS &&
         "wrong config order in ::Segments2Pixels?");
  if (has_watermark_) {
    assert(config_.garbler_inputs[2].type ==
               GarblerInputsType::GARBLER_INPUTS_WATERMARK &&
           "wrong config order in ::Segments2Pixels?");
    assert(config_.garbler_inputs[2].length == nb_pixels && "wrong config!");
  }

  unsigned int nb_inputs = config_.garbler_inputs[1].length;

  // without reserve : 1657472 - 1771623 (ns)
  // with reserve : 1250652 - 1356733 (ns)
  // Now in the .v, ranges are encoded as eg: assign p[75295:75287] = 0;
  // So we really do not need much memory.
  size_t size_to_reserve =
      ((nb_pixels * strlen("assign p[000000] = s[0000];\n")) / 5) + 1000;
  verilog_buf.reserve(size_to_reserve);

  verilog_buf += "// module to convert segments into an image bitmap\n";
  verilog_buf +=
      "// generated by lib_circuits/src/segments2pixels/segments2pixels.cpp\n";
  verilog_buf += "// (c) Interstellar\n\n";

  verilog_buf += "module segment2pixel(s, p);  // convert segments to pixels\n";
  // TODO
  verilog_buf +=
      fmt::format("input [{:d}:0] s; // segments to display\n", nb_inputs - 1);
  verilog_buf +=
      fmt::format("output [{:d}:0] p;  // pixels output\n", nb_pixels - 1);

  // TODO use absl or fmtlib
  size_t pixels_counter = 0;
  for (const auto& it : bitmap_seg_ids_rle) {
    // NOTE: bitmap_seg_ids_rle is RLE encoded
    // - OFF segment(seg_id==-1):   "assign p[7680:0] = 0;"
    // - ON segment(eg seg_id=16):  "assign p[17855:17854] = s[16];"
    auto seg_id = it.value;
    auto len = it.size;
    if (seg_id == -1) {
      // NOTE: range inverted! written as eg [7680:0] not [0:7680]
      verilog_buf += "assign p[";
      verilog_buf += fmt::format_int(pixels_counter + len - 1).str();
      verilog_buf += ":";
      verilog_buf += fmt::format_int(pixels_counter).str();
      verilog_buf += "] = ";
      verilog_buf += "0;\n";
    } else {
      // When a valid seg_id, we CAN NOT write eg "assign p[7456:7412] = s[14];"
      // This is NOT valid verilog, apparently
      // verilator --lint-only: "Operator ASSIGNW expects 47 bits on the Assign
      // RHS, but Assign RHS's SEL generates 1 bits."
      for (uint32_t j = pixels_counter; j < pixels_counter + len; ++j) {
        verilog_buf += "assign p[";
        verilog_buf += fmt::format_int(j).str();
        verilog_buf += "] = ";
        verilog_buf += "s[";
        verilog_buf += fmt::format_int(seg_id).str();
        verilog_buf += "];\n";
      }
    }
    pixels_counter += len;
  }

  verilog_buf += "endmodule";

  return verilog_buf;
}

/**
 * display-main.v and others expect eg:
 *
  `define WIDTH 56
  `define HEIGHT 24
  `define RNDSIZE 9
  `define BITMAP_NB_SEGMENTS 28
 */
template <typename DrawableWhereT>
std::string Segments2Pixels<DrawableWhereT>::GetDefines() const {
  auto verilog_defines = verilog::Defines();
  // NOTE: probably NOT all the config keys are needed on the Verilog side
  // We only need:
  // BITMAP_NB_SEGMENTS
  // WIDTH
  // HEIGHT
  // RNDSIZE
  assert(config_.garbler_inputs[1].type ==
             GarblerInputsType::GARBLER_INPUTS_SEVEN_SEGMENTS &&
         "wrong config order in ::Segments2Pixels?");
  verilog_defines.AddDefine("BITMAP_NB_SEGMENTS",
                            config_.garbler_inputs[1].length);
  verilog_defines.AddDefine("WIDTH", config_.display_config.width);
  verilog_defines.AddDefine("HEIGHT", config_.display_config.height);
  if (has_watermark_) {
    verilog_defines.AddDefine("HAS_WATERMARK");
  }

  assert(config_.evaluator_inputs[0].type ==
             EvaluatorInputsType::EVALUATOR_INPUTS_RND &&
         "wrong config order in ::Segments2Pixels?");
  verilog_defines.AddDefine("RNDSIZE", config_.evaluator_inputs[0].length);

  return verilog_defines.GetDefinesVerilog();
}

/**
 * We could DRY with GetDefines but most of the keys in config are NOT needed on
 * the Verilog side.
 */
template <typename DrawableWhereT>
const SkcdConfig& Segments2Pixels<DrawableWhereT>::GetConfig() const {
  return config_;
}

// "explicit instantiation of all the types the template will be used with"
template class Segments2Pixels<drawable::RelativeBBox>;

}  // namespace interstellar