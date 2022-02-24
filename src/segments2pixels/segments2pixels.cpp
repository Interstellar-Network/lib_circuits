#include "segments2pixels.h"

// TODO Define 'cimg_display' to: '0' to disable display capabilities.
//                           '1' to use the X-Window framework (X11).
//                           '2' to use the Microsoft GDI32 framework.
// else lots of eg "CImg.h:10322: undefined reference to `XMoveWindow'"
#define cimg_display 1
#include <CImg.h>
#include <absl/strings/str_cat.h>
#include <lodepng.h>

#include <filesystem>
#include <vector>

#include "resources.h"
#include "utils/files/utils_files.h"

std::string ReadFromDataDir(boost::filesystem::path input_path) {
  auto data_full_path =
      boost::filesystem::path(interstellar::data_dir) / input_path;
  return interstellar::utils::ReadFile(data_full_path);
}

std::vector<u_int8_t> ReadPng(boost::filesystem::path input_path,
                              unsigned int *width, unsigned int *height) {
  auto png_raw_str = ReadFromDataDir(input_path);
  // copy std::string -> vector<char>
  std::vector<u_int8_t> png_raw_data(png_raw_str.begin(), png_raw_str.end());
  assert(png_raw_data.size() == png_raw_str.size() && "wrong size!");

  // cf
  // https://github.com/lvandeve/lodepng/blob/master/examples/example_decode.cpp

  std::vector<u_int8_t> png_pixels;  // "the raw pixels"

  // decode
  unsigned error = lodepng::decode(png_pixels, *width, *height, png_raw_data);
  if (error) {
    throw std::runtime_error(
        absl::StrCat("lodepng::decode error : ", lodepng_error_text(error)));
  }

  return png_pixels;
}

namespace interstellar {

Segments2Pixels::Segments2Pixels(uint32_t width, uint32_t height)
    : _width(width), _height(height) {
  // Load the .png from data/
  // TODO make the png configurable via ctor
  unsigned int png_width, png_height;
  auto png_pixels = ReadPng("7segs.png", &png_width, &png_height);
  printf("Segments2Pixels %d %d; %d %d", _width, _height, png_width,
         png_height);

  // Prepare the .png that will be "drawn" several times in the final display
  // "Construct image with specified size and initialize pixel values from a
  // memory buffer. " NOTE: using is_shared=True which means png_pixels's data
  // is used directly!
  auto png_img = cimg_library::CImg<uint8_t>(
      png_pixels.data(), png_width, png_height,
      /* size_z */ 1,
      /* size_c = specturm = nb of channels */ 4,
      // NOTE: shared=true + resize:
      // CImg<unsigned char>::assign(): Invalid assignment request of shared
      // instance from specified image (21,21,1,4).
      /* is_shared */ false);
  assert(png_img.size() == png_pixels.size() && "wrong size!");

  // TODO resize to match desired final size in the display
  // TODO keep the .png aspect ratio!
  // "Method of interpolation:
  //   -1 = no interpolation: raw memory resizing.
  //   0 = no interpolation: additional space is filled according to
  //   boundary_conditions. 1 = nearest-neighbor interpolation. 2 = moving
  //   average interpolation. 3 = linear interpolation. 4 = grid interpolation.
  //   5 = cubic interpolation.
  //   6 = lanczos interpolation."
  // png_img.resize(
  //     /* size_x */ _width / 2,
  //     /* size_y = -100 */ _height / 2,
  //     /* size_z = -100 */ png_img.depth(),
  //     /* size_c = -100 */ png_img.spectrum(),
  //     /* 	interpolation_type = 1 */ 1,
  //     /* boundary_conditions = 0 */ 0,
  //     /* centering_x = 0 */ 0,
  //     /* centering_y = 0 */ 0,
  //     /* centering_z = 0 */ 0,
  //     /* centering_c = 0 */ 0);

  png_img.display();

  // Prepare the display with the desired dimensions
  auto display_img = cimg_library::CImg<uint8_t>(
      /* size_x */ _width,
      /* size_y */ _height,
      /* size_z */ 1,
      /* size_c = specturm = nb of channels */ 4);
  assert(display_img.width() == static_cast<int32_t>(_width) &&
         display_img.height() == static_cast<int32_t>(_height) &&
         "wrong dimensions!");

  // Construct the final display by assembling the .png
  // display_img.draw_image(
  //     /* x0 */ 0,
  //     /* y0 */ 0,
  //     /* z0 */ 0,
  //     /* c0 */ 0,
  //     /* sprite */ png_img,
  //     /* opacity */ 1);

  printf("bitmap : %lu", display_img.size());

  display_img.display();  // TODO remove, and set correct define cimg_display,
                          // and remove -lX11

  printf("bitmap : %lu", display_img.size());
}

std::string Segments2Pixels::GenerateVerilog() { return "TODO"; }

}  // namespace interstellar