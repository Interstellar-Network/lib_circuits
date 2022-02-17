#include <filesystem>
#include <lorina/diagnostics.hpp>
#include <lorina/verilog.hpp>
#include <mockturtle/mockturtle.hpp>

#include "resources.h"

int main() {
  mockturtle::aig_network aig;
  auto v_path = std::filesystem::path(interstellar::data_dir) / "verilog" /
                "display-main.v";

  lorina::text_diagnostics consumer;
  lorina::diagnostic_engine diag(&consumer);
  auto const result = lorina::read_verilog(
      v_path.string(), mockturtle::verilog_reader(aig), &diag);
  assert(result == lorina::return_code::success);

  auto const cuts = cut_enumeration(aig);
  aig.foreach_node([&](auto node) {
    std::cout << cuts.cuts(aig.node_to_index(node)) << "\n";
  });

  return 0;
}
