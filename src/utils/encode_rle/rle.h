#pragma once

#include <vector>

namespace interstellar {

namespace utils {

struct RLE_int8_t {
  u_int32_t size;
  int8_t value;

  RLE_int8_t(u_int32_t size, int8_t value);
};

}  // namespace utils

}  // namespace interstellar
