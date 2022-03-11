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

// TODO Define 'cimg_display' to: '0' to disable display capabilities.
//                           '1' to use the X-Window framework (X11).
//                           '2' to use the Microsoft GDI32 framework.
// else lots of eg "CImg.h:10322: undefined reference to `XMoveWindow'"
#define cimg_display 0
#define cimg_use_png
#include <CImg.h>
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

#include "resources.h"
#include "utils/encode_rle/encode_rle.h"
#include "utils/files/utils_files.h"
#include "verilog_defines/verilog_defines.h"

namespace interstellar {

/**
 * return a map {Color: segment ID} like
 * {
 *  #a31cb5: 0,
 *  #b97118: 1,
 * }
 * (not using hex color coding but same idea)
 */
std::unordered_map<ColorRGBA, uint32_t, absl::Hash<ColorRGBA>>
ImgListUniqueColors(const cimg_library::CImg<unsigned char>& img) {
  auto width = img.width(), height = img.height();
  // "wh	Precomputed offset, must be equal to width()*height(). "
  auto wh = img.width() * img.height();
  // "whd	Precomputed offset, must be equal to width()*height()*depth()."
  auto whd = img.width() * img.height() * img.depth();

  auto has_alpha = img.spectrum() == 4;

  std::unordered_set<ColorRGBA, absl::Hash<ColorRGBA>> colors;
  uint32_t nb_colors_found = 0;
  std::unordered_map<ColorRGBA, uint32_t, absl::Hash<ColorRGBA>>
      map_color_to_id;

  for (int x = 0; x < width; ++x) {
    for (int y = 0; y < height; ++y) {
      // NOTE: z range is [0,depth() - 1)
      auto r = img(x, y, 0, 0, wh, whd);
      auto g = img(x, y, 0, 1, wh, whd);
      auto b = img(x, y, 0, 2, wh, whd);
      auto a = has_alpha ? img(x, y, 0, 3, wh, whd) : 0;

      // skip background
      if (r == 0 && g == 0 && b == 0) {
        continue;
      }

      auto color = ColorRGBA(r, g, b, a);

      // if we have a new color:
      // - add it to the "known list"
      // - assign it a unique ID
      auto pair_it_bool =
          colors.emplace(r, g, b, a);  // std::pair<iterator,bool>
      if (pair_it_bool.second) {
        // "true if insertion happened"
        map_color_to_id.emplace(color, nb_colors_found);
        nb_colors_found++;
      }
    }
  }

  return map_color_to_id;
}

// TODO optimize, we are looping on the img once, so we SHOULD RLE-encode then
// and directly reuse the result here
std::vector<utils::RLE_int8_t> ImgReplaceBitmapSegIDs(
    const cimg_library::CImg<unsigned char>& img,
    const std::unordered_map<ColorRGBA, uint32_t, absl::Hash<ColorRGBA>>&
        map_color_to_id) {
  auto width = img.width(), height = img.height();
  // "wh	Precomputed offset, must be equal to width()*height(). "
  auto wh = img.width() * img.height();
  // "whd	Precomputed offset, must be equal to width()*height()*depth()."
  auto whd = img.width() * img.height() * img.depth();

  auto has_alpha = img.spectrum() == 4;

  // CHECK that we indeed only need 32 bits for the segment IDs
  if (map_color_to_id.size() >= std::numeric_limits<uint8_t>::max()) {
    throw std::runtime_error(
        "bitmap_seg_ids too small; requires 16 bits or more!");
  }
  // signed b/c we use "-1" to mean "background"
  std::vector<int8_t> bitmap_seg_ids;
  bitmap_seg_ids.reserve(width * height);

  // CAREFUL: DO NOT switch the order else the final garbled outputs will be
  // rotated 90 degrees. Not a catastrophe but not ideal.
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      // NOTE: z range is [0,depth() - 1)
      auto r = img(x, y, 0, 0, wh, whd);
      auto g = img(x, y, 0, 1, wh, whd);
      auto b = img(x, y, 0, 2, wh, whd);
      auto a = has_alpha ? img(x, y, 0, 3, wh, whd) : 0;

      // shortcut for background: directly insert "-1"
      if (r == 0 && g == 0 && b == 0) {
        bitmap_seg_ids.emplace_back(-1);
        continue;
      }

      auto color = ColorRGBA(r, g, b, a);

      assert(map_color_to_id.at(color) < std::numeric_limits<uint8_t>::max() &&
             "Segment ID does not fit on 8 bits!");
      bitmap_seg_ids.emplace_back(
          static_cast<uint8_t>(map_color_to_id.at(color)));
    }
  }

  assert(bitmap_seg_ids.size() == static_cast<size_t>(width * height) &&
         "wrong size!");

  return utils::compress_rle(bitmap_seg_ids);
}

Segments2Pixels::Segments2Pixels(uint32_t width, uint32_t height)
    : _width(width), _height(height) {
  auto png_img = cimg_library::CImg<unsigned char>();
  png_img.load_png(
      (boost::filesystem::path(interstellar::data_dir) / "7segs.png").c_str());

  // TODO resize to match desired final size in the display
  // keep the .png aspect ratio!
  // "Method of interpolation:
  //   -1 = no interpolation: raw memory resizing.
  //   0 = no interpolation: additional space is filled according to
  //   boundary_conditions. 1 = nearest-neighbor interpolation. 2 = moving
  //   average interpolation. 3 = linear interpolation. 4 = grid interpolation.
  //   5 = cubic interpolation.
  //   6 = lanczos interpolation."
  float png_aspect_ratio = static_cast<float>(png_img.width()) /
                           static_cast<float>(png_img.height());
  // TODO dynamic; eg based on how many we want to draw, and their desired size
  uint32_t png_desired_width =
      _width / 5;  // a fifth of the width looks pretty nice
  uint32_t png_desired_height =
      static_cast<float>(png_desired_width) / png_aspect_ratio;
  png_img.resize(
      /* size_x */ png_desired_width,
      /* size_y = -100 */ png_desired_height,
      /* size_z = -100 */ -100,
      /* size_c = -100 */ -100,
      /* 	interpolation_type = 1 */ 1,
      /* boundary_conditions = 0 */ 0,
      /* centering_x = 0 */ 0,
      /* centering_y = 0 */ 0,
      /* centering_z = 0 */ 0,
      /* centering_c = 0 */ 0);

  assert(ImgListUniqueColors(png_img).size() == 7 &&
         "Something went wrong? Should probably only have found 7 segments");

  // Prepare the display with the desired dimensions
  // MUST use ctor with "value" else
  // "Warning
  //       The allocated pixel buffer is not filled with a default value, and is
  //       likely to contain garbage values. In order to initialize pixel values
  //       during construction (e.g. with 0), use constructor CImg(unsigned
  //       int,unsigned int,unsigned int,unsigned int,T) instead."
  auto display_img = cimg_library::CImg<uint8_t>(
      /* size_x */ _width,
      /* size_y */ _height,
      /* size_z */ 1,
      /* size_c = spectrum = nb of channels */ 4,
      /* value */ 0);
  assert(display_img.width() == static_cast<int32_t>(_width) &&
         display_img.height() == static_cast<int32_t>(_height) &&
         "wrong dimensions!");

  // Construct the final display by assembling the .png
  // TODO move that into helper function that draw several digits where desired
  auto horizontal_margin = (display_img.width() * 0.05);
  auto offset_height = (display_img.height() - png_img.height()) / 2;
  // NOTE: we use opacity to convert "unique in segs.png" to "globally unique"
  // we could do it differently(i.e. more robustly) but this works just fine for
  // now
  // technically this limits to 255 segments in the final image b/c of opacity =
  // ALPHA channel and that is [0-255] but this is more than we need for now
  display_img.draw_image(
      /* x0 */ (display_img.width() / 2) - png_img.width() - horizontal_margin,
      /* y0 */ offset_height,
      /* z0 */ 0,
      /* c0 */ 0,
      /* sprite */ png_img,
      /* opacity */ 1.0f);
  display_img.draw_image(
      /* x0 */ (display_img.width() / 2) + horizontal_margin,
      /* y0 */ offset_height,
      /* z0 */ 0,
      /* c0 */ 0,
      /* sprite */ png_img,
      /* opacity */ 0.99f);

  std::unordered_map<ColorRGBA, uint32_t, absl::Hash<ColorRGBA>>
      map_color_to_seg_id = ImgListUniqueColors(display_img);
  _nb_segments = map_color_to_seg_id.size();
  assert(_nb_segments == ImgListUniqueColors(png_img).size() * 2 &&
         "Something went wrong? Should probably only have found 7*2 segments");

  // TODO add a flag/option to control this; only useful for dev/debug
  // auto bitmap_png = "bitmap.png";
  // display_img.save_png(bitmap_png);
  // LOG(INFO) << "saved : " << std::filesystem::current_path() / bitmap_png;

  // now prepare the final "bitmap"
  // i.e. replace each color pixel by its corresponding segment ID
  _bitmap_seg_ids_rle =
      ImgReplaceBitmapSegIDs(display_img, map_color_to_seg_id);
}

std::string Segments2Pixels::GenerateVerilog() {
  std::string verilog_buf;
  unsigned int nb_inputs = _nb_segments - 1,
               nb_outputs = (_width * _height) - 1;

  // without reserve : 1657472 - 1771623 (ns)
  // with reserve : 1250652 - 1356733 (ns)
  // Now in the .v, ranges are encoded as eg: assign p[75295:75287] = 0;
  // So we really do not need much memory.
  unsigned int nb_pixels = _width * _height;
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
      fmt::format("input [{:d}:0] s; // segments to display\n", nb_inputs);
  verilog_buf +=
      fmt::format("output [{:d}:0] p;  // pixels output\n", nb_outputs);

  // TODO proper values (decode RLE)
  // TODO use absl or fmtlib
  size_t pixels_counter = 0;
  for (const auto& it : _bitmap_seg_ids_rle) {
    // - OFF segment(seg_id==-1):   "assign p[10610:10609] = 0;"
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
std::string Segments2Pixels::GetDefines() {
  // RNDSIZE
  // TODO Check
  // Historically(before the support of variable otp_length), message had
  // RNDSIZE=9, and pinpad RNDSIZE=16
  // math.ceil(0.5 * math.sqrt(8 * otp_length * message_seg + 1) + 1)
  auto rndsize = static_cast<unsigned int>(
      std::max(std::ceil(0.5 * std::sqrt(8 * _nb_segments + 1) + 1), 9.));

  auto verilog_defines = verilog::Defines();
  verilog_defines.AddDefine("WIDTH", _width);
  verilog_defines.AddDefine("HEIGHT", _height);
  verilog_defines.AddDefine("BITMAP_NB_SEGMENTS", _nb_segments);
  verilog_defines.AddDefine("RNDSIZE", rndsize);

  return verilog_defines.GetDefinesVerilog();
}

}  // namespace interstellar