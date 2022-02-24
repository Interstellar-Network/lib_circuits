#pragma once

#include <string>

#include "utils/encode_rle/rle.h"

namespace interstellar {

// TODO internal to the class?
typedef std::tuple<u_int8_t /*r*/, u_int8_t /*g*/, u_int8_t /*b*/,
                   u_int8_t /*a*/>
    ColorRGBA;

/**
 * Helper class that is the step [1] in circuit gen pipeline
 *
 * Allows to:
 * - generate a "bitmap" of the desired size/resolution; then converts it to a
 * .v circuit
 * - generate the define.v containing the Verilog macro used by all
 * xorexpand/rndswitch/display-main
 *
 * NOTE: b/c this class DOES NOT use Verilog/ABC, all is done in buffers(eg
 * string) instead of using files
 */
class Segments2Pixels {
 public:
  Segments2Pixels(uint32_t width, uint32_t height);

  /**
   * Return: buffer containing a valid Verilog .v circuit
   *
   * It is used to compile the final circuit "display-main.v"
   */
  std::string GenerateVerilog();

  std::string GetDefines();

 private:
  uint32_t _width, _height, _nb_segments;

  std::vector<utils::RLE_int8_t> _bitmap_seg_ids_rle;
};

}  // namespace interstellar
