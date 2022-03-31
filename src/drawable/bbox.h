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

#include <utility>

#include "points.h"

namespace interstellar {

namespace drawable {

/**
 * Interface for the various types of RelativeBBox: basic, rotated, etc
 * That is WHERE to draw; the WHAT is in drawable.h
 *
 * How to add translation/rotation/etc:
 * - "apply" the transform in IsInBBox
 *  It means map the coords given in inputs to the coords without the
 * transformation. This will virtually move the whole BBox.
 * - "remove it" in GetRelCoordsLocalFromRelCoordsWorld
 *  It means: from a coords inside the "virtually moved" BBox, we want the
 * coords inside the original BBox.
 *
 * WARNING: apply/remove the transform is the same operation, ie uses the same
 * sign ! DO NOT eg coords + offset in IsInBBox, and then coords - offset in
 * GetLocalRelCoordsFromWorldRelCoords BOTH MUST be the same operation !
 *
 */
class RelativeBBoxInterface {
 public:
  /**
   * Point2DRelative: coords in the whole bitmap, there can be multiple digits
   * in the bitmap
   */
  virtual bool IsInBBox(Point2DRelative rel_coords_world) const = 0;

  RelativeBBoxInterface();

  // Disable copy semantics.
  RelativeBBoxInterface(const RelativeBBoxInterface&) = delete;
  RelativeBBoxInterface& operator=(const RelativeBBoxInterface&) = delete;
  // MOVE ONLY
  RelativeBBoxInterface& operator=(RelativeBBoxInterface&& other) = default;
  RelativeBBoxInterface(RelativeBBoxInterface&& other) = default;

  virtual ~RelativeBBoxInterface() {}

  /**
   * Converts RelativeCoords IN THE FINAL BITMAP to ones relative to this(ie
   * 0-1).
   * "Cancel" any rotation/translation/etc
   * PREREQ: IsInBBox(global_rel_coords) else throw
   *
   * coords: expected to be inside one of the digit
   */
  virtual Point2DRelative GetRelCoordsLocalFromRelCoordsWorld(
      Point2DRelative rel_coords_world) const = 0;
};

class RelativeBBox : public RelativeBBoxInterface {
 public:
  RelativeBBox(Point2DRelative lower_left, Point2DRelative upper_right);

  virtual bool IsInBBox(Point2DRelative rel_coords_world) const override;

  virtual Point2DRelative GetRelCoordsLocalFromRelCoordsWorld(
      Point2DRelative rel_coords_world) const override;

 private:
  Point2DRelative lower_left_world_, upper_right_world_;
};

}  //   namespace drawable

}  // namespace interstellar