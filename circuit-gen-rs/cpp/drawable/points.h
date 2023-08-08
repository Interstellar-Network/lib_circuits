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

namespace interstellar {

namespace drawable {

/**
 * Point(2D) in pixel coordinates
 */
class Point2DInPixels {
 public:
  unsigned int x, y;

  Point2DInPixels(unsigned int x, unsigned int y);
};

/**
 * Point(2D) in relative coordinates
 */
class Point2DRelative {
 public:
  float x, y;

  Point2DRelative(float x, float y);

  Point2DRelative() { x = y = 0.f; }
};

}  //   namespace drawable

}  // namespace interstellar