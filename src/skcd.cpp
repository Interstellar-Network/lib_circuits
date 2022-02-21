#include "skcd.h"

#include <utility>

namespace interstellar {

Skcd::Skcd(BlifParser &&blif_parser)
    : m(blif_parser.Getm()),
      n(blif_parser.Getn()),
      q(blif_parser.Getq()),
      A(std::move(blif_parser.GetA())),
      B(std::move(blif_parser.GetB())),
      GO(std::move(blif_parser.GetGO())),
      GT(std::move(blif_parser.GetGT())),
      O(std::move(blif_parser.GetO())) {}

}  // namespace interstellar