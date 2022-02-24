#pragma once

#include <vector>

#include "rle.h"

namespace interstellar {

namespace utils {

std::vector<RLE_int8_t> compress_rle(const std::vector<int8_t>& vect);

}  // namespace utils

}  // namespace interstellar
