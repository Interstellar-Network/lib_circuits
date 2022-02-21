#include "circuit_lib.h"

#include "segments2pixels.h"

namespace interstellar {

namespace CircuitPipeline {

Skcd GenerateSkcd() {
  // [1] generate Verilog segments2pixels.v
  auto segment2pixels = Segments2Pixels(42, 42);
  auto segment2pixels_v_str = segment2pixels.GenerateVerilog();

  // [?]

  // convert .blif.blif -> .skcd
  auto blif_parser = BlifParser();
  blif_parser.ParseBuffer(segment2pixels_v_str, true);

  // [?]
  return Skcd(std::move(blif_parser));
}

}  // namespace CircuitPipeline

}  // namespace interstellar