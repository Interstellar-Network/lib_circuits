#include "encode_rle.h"

#include <cassert>

namespace interstellar {

namespace utils {

RLE_int8_t::RLE_int8_t(u_int32_t size, int8_t value)
    : size(size), value(value) {}

/**
 * Run Lenght Encode a vector(ie a bitmap)
 *
 * https://stackoverflow.com/questions/52606433/run-length-encoding-in-c
 *
 * WARNING: the values should fit on 8 bits signed, but the counts WILL NOT!
 */
std::vector<RLE_int8_t> compress_rle(const std::vector<int8_t>& vect) {
  const size_t vect_size = vect.size();

  std::vector<RLE_int8_t> result_rle;
  // heuristic(a bit pessimistic), we assume the RLE will indeed compress
  result_rle.reserve(vect_size);

  for (size_t i = 0; i < vect_size; ++i) {
    int count = 1;

    while ((i + 1 < vect_size) && (vect[i] == vect[i + 1])) {
      count++;
      i++;
    }

    result_rle.emplace_back(count, vect[i]);
  }

  assert(result_rle.size() <= vect_size && "compress_rle: was resized!");

#ifndef NDEBUG
  size_t nb_elem = 0;
  for (int32_t i = 0, s = result_rle.size(); i < s; i++) {
    nb_elem += result_rle[i].size;
  }
  assert(nb_elem == vect_size && "compress_rle: wrong size!");
#endif

  return result_rle;
}

}  // namespace utils

}  // namespace interstellar
