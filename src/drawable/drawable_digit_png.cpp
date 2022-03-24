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

#include "drawable_digit_png.h"

// TODO Define 'cimg_display' to: '0' to disable display capabilities.
//                           '1' to use the X-Window framework (X11).
//                           '2' to use the Microsoft GDI32 framework.
// else lots of eg "CImg.h:10322: undefined reference to `XMoveWindow'"
#define cimg_display 0
#define cimg_use_png
#include <CImg.h>

#include <boost/filesystem.hpp>

#include "map_color_to_segid.h"
#include "resources.h"
#include "utils/files/utils_files.h"

namespace {

using interstellar::drawable::ColorRGBA;
using interstellar::drawable::SegmentID;

// TODO optimize, we are looping on the img once, so we SHOULD RLE-encode then
// and directly reuse the result here
std::vector<SegmentID> ImgReplaceBitmapSegIDs(
    const cimg_library::CImg<unsigned char>& img,
    const absl::flat_hash_map<ColorRGBA, uint32_t>& map_color_to_seg_id) {
  auto width = img.width(), height = img.height();
  // "wh	Precomputed offset, must be equal to width()*height(). "
  auto wh = img.width() * img.height();
  // "whd	Precomputed offset, must be equal to width()*height()*depth()."
  auto whd = img.width() * img.height() * img.depth();

  auto has_alpha = img.spectrum() == 4;

  // CHECK that we indeed only need 32 bits for the segment IDs
  if (map_color_to_seg_id.size() >= std::numeric_limits<uint8_t>::max()) {
    throw std::runtime_error(
        "bitmap_seg_ids too small; requires 16 bits or more!");
  }
  // signed b/c we use "-1" to mean "background"
  std::vector<SegmentID> bitmap_seg_ids;
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

      assert(map_color_to_seg_id.at(color) <
                 std::numeric_limits<uint8_t>::max() &&
             "Segment ID does not fit on 8 bits!");
      bitmap_seg_ids.emplace_back(map_color_to_seg_id.at(color));

      // TODO CHECK that the "current number of segment ID found"
      // is NOT > nb_segments_per_digit_
      // else THROW
    }
  }

  assert(bitmap_seg_ids.size() == static_cast<size_t>(width * height) &&
         "wrong size!");

  return bitmap_seg_ids;
}

}  // namespace

namespace interstellar {

namespace drawable {

DrawableDigitPng::DrawableDigitPng()
    // DigitSegmentsType MUST match with PackmsgDigitSegmentsType in lib_garble
    : IDrawableSegmentedDigitRelCoordsLocal(DigitSegmentsType::seven_segs) {
  auto png_img = cimg_library::CImg<unsigned char>();
  png_img.load_png(
      (boost::filesystem::path(interstellar::data_dir) / "7segs.png").c_str());
  png_width_ = png_img.width();
  png_height_ = png_img.height();

  auto map_color_to_seg_id = GetMapColorToSegId();
  img_seg_ids_ = ImgReplaceBitmapSegIDs(png_img, map_color_to_seg_id);
  assert(img_seg_ids_.size() == png_width_ * png_height_ &&
         "DrawableDigitPng: size mismatch!");
}

SegmentID DrawableDigitPng::GetSegmentID(
    Point2DRelative rel_coords_local) const {
  // convert the relative coords into pixel coords using the internal png_width
  uint32_t px_coords_x = png_width_ * rel_coords_local.x;
  uint32_t px_coords_y = png_height_ * rel_coords_local.y;

  // the data is stored as a 1D array so convert 2D->1D
  // NOTE: min: truncate to the size of the bitmap b/c without this it COULD go
  // outside depending on float->int conversion
  auto px = std::min(px_coords_x + px_coords_y * png_width_,
                     png_width_ * png_height_ - 1);
  assert(px < img_seg_ids_.size() &&
         "DrawableDigitPng::GetSegmentID: invalid px!");

  return img_seg_ids_[px];
}

}  //   namespace drawable

}  // namespace interstellar