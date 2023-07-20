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

#pragma once

#include "drawable.h"

namespace interstellar {

namespace drawable {

class DrawableDigitPng : public IDrawableSegmentedDigitRelCoordsLocal {
 public:
  DrawableDigitPng();

  // Disable copy semantics.
  DrawableDigitPng(const DrawableDigitPng&) = delete;
  DrawableDigitPng& operator=(const DrawableDigitPng&) = delete;

  virtual SegmentID GetSegmentID(
      Point2DRelative rel_coords_local) const override;

 private:
  u_int32_t png_width_, png_height_;
  std::vector<SegmentID> img_seg_ids_;
};

}  //   namespace drawable

}  // namespace interstellar