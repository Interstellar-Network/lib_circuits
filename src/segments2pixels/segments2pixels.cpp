#include "segments2pixels.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "resources.h"

// TODO move into utils_file + separate lib
std::string ReadVerilogFromDataDir(const std::string& v_file_name) {
  // TODO avoid useless string copy x2?
  // TODO error handling
  auto v_path =
      std::filesystem::path(interstellar::data_dir) / "verilog" / v_file_name;
  std::ifstream input_stream_v(v_path, std::ios_base::binary);
  std::ostringstream sstr;
  sstr << input_stream_v.rdbuf() << "\n";
  return sstr.str();
}

namespace interstellar {

Segments2Pixels::Segments2Pixels(uint32_t width, uint32_t height)
    : _width(width), _height(height) {
  // TODO use CImg/skia to parse data/7segs.png and assemble as needed
  printf("Segments2Pixels %d %d", _width, _height);
}

std::string Segments2Pixels::GenerateVerilog() {
  // TODO read the bitmap generated in the ctor, and generate a Verilog buffer
  return ReadVerilogFromDataDir("segment2pixels.v");
}

}  // namespace interstellar