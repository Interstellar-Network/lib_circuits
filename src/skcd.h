#pragma once

#include <map>
#include <vector>

#include "blif_parser.h"
#include "gate_types.h"

namespace interstellar {

/**
 * This is the second step of the circuit pipeline:
 * BlifParser -> Skcd -> GarbledCircuit -> ParallelGarbledCircuit
 *
 * It can only be constructed from a (moved) BlifParser.
 *
 * It replaces the "writing an intermediate .skcd file", but that induces a
 * light coupling between the circuit generator and the garbler.
 *
 * Currently header-only, to avoid compiling the .cpp for both circuit_lib and
 * libGarble
 *
 * NOTE: seeing the ctor, this class may seem useless, but without it we would
 * need to include blif_parser.h in the garbling lib, and thus pull a lot of
 * unwanted deps.
 */
class Skcd {
 public:
  // TODO private
  unsigned int m, n, q;

  std::vector<uint32_t> A;       // gates first inputs
  std::vector<uint32_t> B;       // gates second inputs
  std::vector<uint32_t> GO;      // gates outputs
  std::vector<SkcdGateType> GT;  // gates types
  std::vector<uint32_t> O;       // outputs

  /**
   * Standard ctor called when generating circuits
   * It can only be constructed from a (moved) BlifParser.
   */
  Skcd(BlifParser &&blif_parser);

  // NOTE: usually obtained from the SkcdCache, so must be copyable
};

}  // namespace interstellar