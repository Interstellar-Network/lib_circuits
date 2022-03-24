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
#include "drawable.h"

#include <cassert>

namespace {

using interstellar::drawable::DigitSegmentsType;

/**
 * return: the number of segments PER digit for a given DigitSegmentsType
 * eg 7 segments in general
 */
uint32_t GetDigitSegmentsTypeNbSegments(DigitSegmentsType segments_type) {
  switch (segments_type) {
    case DigitSegmentsType::seven_segs:
      return 7;
      break;
    case DigitSegmentsType::fourteen_segs:
      return 14;
      break;
    case DigitSegmentsType::iching:
      return 18;
      break;
  }

  // GCC: "error: control reaches end of non-void function
  // [-Werror=return-type]"
  __builtin_unreachable();
}

}  // anonymous namespace

namespace interstellar {

namespace drawable {

IDrawableSegmentedDigitRelCoordsLocal::IDrawableSegmentedDigitRelCoordsLocal(
    DigitSegmentsType segments_type)
    : segments_type_(segments_type),
      nb_segments_per_digit_(GetDigitSegmentsTypeNbSegments(segments_type_)) {}

DigitSegmentsType IDrawableSegmentedDigitRelCoordsLocal::GetType() const {
  return segments_type_;
}

uint32_t IDrawableSegmentedDigitRelCoordsLocal::GetNbSegments() const {
  return nb_segments_per_digit_;
}

Drawable::Drawable(std::unique_ptr<const RelativeBBoxInterface>&& where_to_draw,
                   const IDrawableSegmentedDigitRelCoordsLocal& what_to_draw)
    : where_to_draw_(std::move(where_to_draw)), what_to_draw_(what_to_draw) {}

const IDrawableSegmentedDigitRelCoordsLocal& Drawable::What() const {
  return what_to_draw_;
}

const RelativeBBoxInterface& Drawable::Where() const { return *where_to_draw_; }

/**
 * NOTE: try NOT to make this part too Verilog-specific because that way we can
 * write the output to a bitmap/png which is easier for dev/debug.
 * Technically this COULD directly return a map of some sort:
 * pixel(x1,y1) = segment0
 * pixel(x2,y2) = segment1
 * and assume the UNreturned pixel are background(ie NOT a SegmentID)
 */
std::vector<SegmentID> Draw(const std::vector<drawable::Drawable>& drawables,
                            u_int32_t width, u_int32_t height) {
  std::vector<SegmentID> img_seg_ids;
  img_seg_ids.reserve(width * height);

  // CAREFUL: DO NOT switch the order else the final garbled outputs will be
  // rotated 90 degrees. Not a catastrophe but not ideal.
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      drawable::Point2DRelative rel_coords_world(
          static_cast<float>(x) / static_cast<float>(width),
          static_cast<float>(y) / static_cast<float>(height));

      // Find a drawable, if the rel_coords(ie the current pixel) is indeed on
      // one else it means it is background
      bool is_background = true;
      uint32_t offset_nb_segments = 0;
      for (const auto& drawable : drawables) {
        if (drawable.Where().IsInBBox(rel_coords_world)) {
          is_background = false;

          auto rel_coords_local =
              drawable.Where().GetRelCoordsLocalFromRelCoordsWorld(
                  rel_coords_world);
          auto local_seg_id = drawable.What().GetSegmentID(rel_coords_local);
          if (local_seg_id != -1) {
            // REALLY IMPORTANT
            // MUST convert the local_seg_id(eg usually 0-6 for 7 segs)
            // to a global one UNIQUE in the final bitmap
            img_seg_ids.emplace_back(
                SegmentID(offset_nb_segments + local_seg_id));
          } else {
            // background (in the current drawable)
            img_seg_ids.emplace_back(-1);
          }

          // we COULD overwrite with another Drawable in case of overlap but
          // what is the point; we might as well stop processing
          break;
        }

        offset_nb_segments += drawable.What().GetNbSegments();
      }

      // background (in the global bitmap)
      if (is_background) {
        img_seg_ids.emplace_back(-1);
      }
    }
  }

  assert(img_seg_ids.size() == width * height && "Draw: missing pixels!");
  return img_seg_ids;
}

}  //   namespace drawable

}  // namespace interstellar