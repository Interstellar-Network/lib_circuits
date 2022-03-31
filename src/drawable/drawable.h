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

#include <memory>
#include <vector>

#include "bbox.h"
#include "points.h"

namespace interstellar {

namespace drawable {

// DO NOT MODIFY THE ORDER OR REMOVE ONE
// It MUST MATCH lib_garble/src/packmsg/packmsg_utils.h
enum class DigitSegmentsType { seven_segs, fourteen_segs, iching };

/**
 * SegmentID associated with a pixel in the "display circuit"
 *
 * eg data/7segs.png has 7 segments so SegmentID from 0 to 6.
 * But a bunch of pixels have the same SegmentID (eg all with a given color).
 *
 * WARNING: right now int8, so 127 max is enough; current max is 10 * 7 = 70
 * for the pinpad; message is 4 * 14.
 * That may change in the future!
 */
using SegmentID = int32_t;

/**
 * Interface for all the "SegmentedDigits"
 * That is WHAT to draw; the WHERE is in bbox.h
 *
 * Currently used only by "7segs.png".
 *
 * Only two methods: GetSegmentIds, GetNbSegments
 *
 * NOTE: it is assumed that the segment ids are [0:N-1], with N =
 * GetNbSegments() but there is no check anywhere in this class b/c abstract.
 * The callers should check this.
 */
class IDrawableSegmentedDigitRelCoordsLocal {
 public:
  IDrawableSegmentedDigitRelCoordsLocal(DigitSegmentsType type);

  // Disable copy semantics.
  IDrawableSegmentedDigitRelCoordsLocal(
      const IDrawableSegmentedDigitRelCoordsLocal&) = delete;
  IDrawableSegmentedDigitRelCoordsLocal& operator=(
      const IDrawableSegmentedDigitRelCoordsLocal&) = delete;

  /**
   * Return a list of 'segment ids'(or -1 if not a segment) in this "bitmap"
   * NOTE/IMPORTANT: the segid are in the CURRENT bitmap, without any
   * consideration for the factor that a final image is typically composed of
   * several digits.
   * The transformation 'local seg id' to 'global seg id' is
   * done in DisplayBitmap, which know everything that composes the bitmap.
   *
   * -1 means 'not a segment'
   *
   * param: rel_coords_local: LOCAL RELATIVE coords
   * It SHOULD b/w 0.0 and 1.0 when iterating!
   *
   * NOTE: the 'digits' are expected to be global object(read-only) reused
   * accross the whole program, so return a ref.
   * NOTE2: NOT const, b/c implementations SHOULD return from a cache
   */
  virtual SegmentID GetSegmentID(Point2DRelative rel_coords_local) const = 0;

  DigitSegmentsType GetType() const;

  /**
   * Returns the number of segments in the "digit".
   * Historically it was a 7 segments display, but now it can be anything
   * eg 14 segments for alphabet, i-chings, anything really
   */
  virtual uint32_t GetNbSegments() const;

  virtual ~IDrawableSegmentedDigitRelCoordsLocal() {}

 protected:
  DigitSegmentsType segments_type_;
  uint32_t nb_segments_per_digit_;
};

/**
 * Binds the WHAT to draw and the WHERE.
 * This is what is given to Segments2Pixels
 */
template <typename DrawableWhereT>  // DrawableWhereT: should be derived from
                                    // RelativeBBoxInterface
class Drawable {
 public:
  static_assert(std::is_base_of_v<RelativeBBoxInterface, DrawableWhereT>,
                "Drawable<DrawableWhereT> DrawableWhereT MUST derived from "
                "RelativeBBoxInterface");

  /**
   * WARNING you MUST keep "what_to_draw" alive for the whole lifetime of
   * durable!
   * But "what_to_draw" is JUST A REFERENCE!
   * Usually you will want to keep it around and reuse b/w circuit gen (eg use a
   * local static?)
   */
  Drawable(DrawableWhereT&& where_to_draw,
           const IDrawableSegmentedDigitRelCoordsLocal& what_to_draw);

  // Disable copy semantics.
  Drawable(const Drawable&) = delete;
  Drawable& operator=(const Drawable&) = delete;
  // Allow move(needed b/c usually constructed with eg
  // drawables.emplace_back(std::make_unique<drawable::RelativeBBox>(
  //  drawable::Point2DRelative(0.25f, 0.1f),
  //  drawable::Point2DRelative(0.45f, 0.9f)),
  //  seven_segs_png);)
  Drawable(Drawable&&) = default;

  const IDrawableSegmentedDigitRelCoordsLocal& What() const;
  const DrawableWhereT& Where() const;

 private:
  // TODO const? but not really possible
  DrawableWhereT where_to_draw_;
  const IDrawableSegmentedDigitRelCoordsLocal& what_to_draw_;
};

/**
 * return: a bitmap(-like) image whose individual "pixels" are SegmentID
 */
template <typename DrawableWhereT>
std::vector<SegmentID> Draw(
    const std::vector<drawable::Drawable<DrawableWhereT>>& drawables,
    u_int32_t width, u_int32_t height);

}  //   namespace drawable

}  // namespace interstellar