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

#include "bbox.h"

#include <cassert>

namespace interstellar {

namespace drawable {

RelativeBBoxInterface::RelativeBBoxInterface() {}

RelativeBBox::RelativeBBox(float lower_left_x, float lower_left_y,
                           float upper_right_x, float upper_right_y)
    : RelativeBBox(Point2DRelative(lower_left_x, lower_left_y),
                   Point2DRelative(upper_right_x, upper_right_y)) {}

RelativeBBox::RelativeBBox(Point2DRelative lower_left_world,
                           Point2DRelative upper_right_world)
    : lower_left_world_(lower_left_world),
      upper_right_world_(upper_right_world) {
  // CHECK make sure the bbox is valid
  // TODO? exception instead?
  assert(lower_left_world_.x < upper_right_world_.x &&
         lower_left_world_.y < upper_right_world_.y);
}

bool RelativeBBox::IsInBBox(Point2DRelative rel_coords_world) const {
  // NO assert: we obvously want it to work even if outside the bbox
  return (rel_coords_world.x >= lower_left_world_.x) &&
         (rel_coords_world.x <= upper_right_world_.x) &&
         (rel_coords_world.y >= lower_left_world_.y) &&
         (rel_coords_world.y <= upper_right_world_.y);
}

/**
 * example: RelativeBBox({0.6, 0.2}, {0.8, 0.9})
 * eg 7segs centered-ish vertically, and slightly offset to the middle right
 * rel_coords_world:
 * - Xworld 0.6 -> Xlocal 0.0    |   Yworld 0.2 -> Ylocal 0.0
 * - Xworld 0.8 -> Xlocal 1.0    |   Yworld 0.9 -> Ylocal 1.0
 */
Point2DRelative RelativeBBox::GetRelCoordsLocalFromRelCoordsWorld(
    Point2DRelative rel_coords_world) const {
  // The caller SHOULD have checked before calling this
  assert(IsInBBox(rel_coords_world) &&
         "GetRelCoordsLocalFromRelCoordsWorld: prereq IsInBBox failed!");

  // - first: TRANSLATE WORLD->LOCAL by substracting lower_left_world_
  // - second: SCALE WORLD->LOCAL by dividing by the size on the axis
  return Point2DRelative((rel_coords_world.x - lower_left_world_.x) /
                             (upper_right_world_.x - lower_left_world_.x),
                         (rel_coords_world.y - lower_left_world_.y) /
                             (upper_right_world_.y - lower_left_world_.y));
}

}  //   namespace drawable

}  // namespace interstellar