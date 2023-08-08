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

#include "segments2pixels/segments2pixels.h"

#include <absl/strings/str_cat.h>
#include <absl/strings/str_split.h>
#include <glog/logging.h>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <thread>

#include "drawable/drawable.h"
#include "drawable/drawable_digit_png.h"
#include "test_resources.h"
#include "utils/files/utils_files.h"

using interstellar::drawable::Drawable;
using interstellar::drawable::DrawableDigitPng;
using interstellar::drawable::Point2DRelative;
using interstellar::drawable::RelativeBBox;

TEST(Segments2PixelsTest, BasicDisplayOk) {
  // TODO get that from a static? or better to let the caller do that?
  DrawableDigitPng seven_segs_png;
  std::vector<Drawable<RelativeBBox>> drawables;
  drawables.emplace_back(
      RelativeBBox(Point2DRelative(0.25f, 0.1f), Point2DRelative(0.45f, 0.9f)),
      seven_segs_png);
  drawables.emplace_back(
      RelativeBBox(Point2DRelative(0.55f, 0.1f), Point2DRelative(0.75f, 0.9f)),
      seven_segs_png);

  // ~ minimal size that still displays all the segments in lib_garble's eval
  auto segments2pixels =
      interstellar::Segments2Pixels(120, 52, drawables, true);

  auto expected_str = interstellar::utils::ReadFile(
      absl::StrCat(interstellar::test::test_data_dir,
                   "/display_120x52_2digits/segments2pixels.v"));

  // NOTE: internally the "defines map" is NOT ordered, so we split+sort to
  // compare
  auto res_defines_lines =
      absl::StrSplit(segments2pixels.GetDefines(), "\n", absl::SkipEmpty());
  ASSERT_THAT(res_defines_lines, testing::UnorderedElementsAreArray({
                                     "`define WIDTH 120",
                                     "`define BITMAP_NB_SEGMENTS 14",
                                     "`define RNDSIZE 140",
                                     "`define HEIGHT 52",
                                     "`define HAS_WATERMARK",
                                 }));

  EXPECT_EQ(segments2pixels.GenerateVerilog(), expected_str);
}