// Copyright (c) 2018 Skeyecode. All rights reserved.
// author : Nathan Prat
// date : 2018-03-05

#pragma once

#include <cmath>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "circuit_data.h"
#include "gate_types.h"
#include "my_random.h"

namespace interstellar {

class Label {
 public:
  std::string_view str_v;
};

/**
 * Basic .blif parser.
 * Used to parse the blif juste before conversion to .skcd
 *
 * WARNING: only parses a subset of .blif !
 */
class BlifParser {
 public:
  BlifParser() { random_ = std::make_shared<Random>(); }

  // DEV/TEST
  // Used during tests to use a fake PRNG, needed to have a determistic output
  // of Parse*() with "zero=True"
  // TODO remove when NOT a test build
  BlifParser(std::shared_ptr<RandomInterface> random) : random_(random) {}

  // Disable copy semantics.
  BlifParser(const BlifParser &) = delete;
  BlifParser &operator=(const BlifParser &) = delete;
  // noncopyable, but we still need the moves
  BlifParser(BlifParser &&) = default;
  BlifParser &operator=(BlifParser &&) = default;

  // TODO handle the '-z' option, see lib_python
  // TODO return a struct/class
  // TODO string_view
  void ParseBuffer(std::string_view blif_buffer, bool zero);

  unsigned int Getn() const { return n_; }

  unsigned int Getm() const { return m_; }

  unsigned int Getq() const { return q_; }

  // WARNING: the functions below return REF on the internal vectors;
  // They are used when constructing the Skcd, by move so ref is fine.

  const std::vector<unsigned int> &GetA() const { return A_; }

  const std::vector<unsigned int> &GetB() const { return B_; }

  const std::vector<unsigned int> &GetGO() const { return GO_; }

  const std::vector<SkcdGateType> &GetGT() const { return GT_; }

  const std::vector<unsigned int> &GetO() const { return O_; }

  const CircuitData &GetCircuitData() const { return circuit_data_; }

 private:
  // Basic deps injection to use a fake PRNG during tests
  std::shared_ptr<RandomInterface> random_;

  unsigned int n_ = 0;  // inputs number
  unsigned int m_ = 0;  // outputs number
  unsigned int q_ = 0;  // gates number

  // Vector of "label index"(except GT)
  std::vector<unsigned int> A_;   // gates first inputs
  std::vector<unsigned int> B_;   // gates second inputs
  std::vector<unsigned int> GO_;  // gates outputs
  std::vector<SkcdGateType> GT_;  // gates types
  std::vector<unsigned int> O_;   // outputs

  // Associate a label(as hashed string/string_view) to a index(0-N)
  // TODO(unordered_map?) replace by array LUT?
  std::unordered_map<uint64_t, size_t> labels_map_;
  // size_t current_label_count_ = 0;

  // TODO? but does it make sense when using an temporary file .skcd?
  CircuitData circuit_data_;

  /**
   * "Unlike other string types, you should pass string_view by value just like
 you would an int or a double because string_view is a small value."
   */
  uint32_t GetLabel(std::string_view lbl);
};

}  // namespace interstellar