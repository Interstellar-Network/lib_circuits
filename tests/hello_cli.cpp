#include <filesystem>
#include <lorina/diagnostics.hpp>
#include <lorina/verilog.hpp>
#include <mockturtle/mockturtle.hpp>

#include "resources.h"

// TODO move into lib
// TODO read values from "config", eg add CircuitConfig class with internal map?
void AddVerilogDefines(std::ofstream *output_stream_v) {
  (*output_stream_v) << "`define RNDSIZE 42\n";
  (*output_stream_v) << "`define BITMAP_NB_SEGMENTS 42\n";
  (*output_stream_v) << "`define WIDTH 42\n";
  (*output_stream_v) << "`define HEIGHT 42\n";
  (*output_stream_v) << "\n";
}

int main() {
  // concat all the data/verilog/*.v into this tmpfile
  // TODO use /tmp + unique name
  // std::FILE* tmpf = std::tmpfile();
  std::string output_name_v = "output.v";

  {
    std::ofstream output_stream_v(output_name_v, std::ios::binary);

    AddVerilogDefines(&output_stream_v);

    // WARNING: order matters for verilog compilation !
    for (auto v_file_name : {
             "xorexpand.v",
             "rndswitch.v",
             "display-main.v",
         }) {
      auto v_path = std::filesystem::path(interstellar::data_dir) / "verilog" /
                    v_file_name;
      std::ifstream input_stream_v(v_path, std::ios_base::binary);

      output_stream_v << input_stream_v.rdbuf();
    }
  }

  lorina::text_diagnostics consumer;
  lorina::diagnostic_engine diag(&consumer);
  mockturtle::aig_network aig;
  auto const result = lorina::read_verilog(
      output_name_v, mockturtle::verilog_reader(aig), &diag);
  assert(result == lorina::return_code::success);

  auto const cuts = cut_enumeration(aig);
  aig.foreach_node([&](auto node) {
    std::cout << cuts.cuts(aig.node_to_index(node)) << "\n";
  });

  return 0;
}
