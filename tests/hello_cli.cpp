#include <absl/strings/str_cat.h>
#include <absl/strings/str_replace.h>

#include <filesystem>
#include <lorina/diagnostics.hpp>
#include <lorina/verilog.hpp>
#include <mockturtle/mockturtle.hpp>

#include "resources.h"

// TODO move into lib
// TODO read values from "config", eg add CircuitConfig class with internal map?
// void AddVerilogDefines(std::ofstream* output_stream_v) {
//   // (*output_stream_v) << "`define RNDSIZE 42\n";
//   // (*output_stream_v) << "`define BITMAP_NB_SEGMENTS 42\n";
//   // (*output_stream_v) << "`define WIDTH 42\n";
//   // (*output_stream_v) << "`define HEIGHT 42\n";
//   // NOTE: lorina can not parse `DEFINE so we put everything in a module
//   (*output_stream_v) << "module defines;\n";
//   // (*output_stream_v) << "integer RNDSIZE = 42;\n";
//   // (*output_stream_v) << "integer BITMAP_NB_SEGMENTS = 42;\n";
//   // (*output_stream_v) << "integer WIDTH = 42;\n";
//   // (*output_stream_v) << "integer HEIGHT = 42;\n";
//   (*output_stream_v) << "reg RNDSIZE = 42;\n";
//   (*output_stream_v) << "reg BITMAP_NB_SEGMENTS = 42;\n";
//   (*output_stream_v) << "reg WIDTH = 42;\n";
//   (*output_stream_v) << "reg HEIGHT = 42;\n";
//   (*output_stream_v) << "endmodule\n";
//   (*output_stream_v) << "\n";
// }

/**
 * lorina's verilog parser DOES NOT support `define
 * so we do a basic find-and-replace ourself
 */
void ProcessDefines(std::string* str_v) {
  // TODO pass a map argument eg {"`define AAA": 42}
  int count = absl::StrReplaceAll({{"`RNDSIZE", "42"},
                                   {"`BITMAP_NB_SEGMENTS", "42"},
                                   {"`WIDTH", "42"},
                                   {"`HEIGHT", "42"}},
                                  str_v);
  assert(count && "No replacement occurered; is there a `define missing?");
}

// https://github.com/hriener/lorina/blob/e05e81df2a8c0317739875795fab00d26a4bc264/test/verilog.cpp
class simple_verilog_reader : public lorina::verilog_reader {
 public:
  struct module_info {
    std::string module_name;
    std::string inst_name;
    std::vector<std::string> params;
    std::vector<std::pair<std::string, std::string>> args;
  };

 public:
  simple_verilog_reader() {}

  void on_module_header(const std::string& module_name,
                        const std::vector<std::string>& inouts) const override {
    (void)module_name;
    (void)inouts;
  }

  void on_inputs(const std::vector<std::string>& inputs,
                 std::string const& size = "") const override {
    (void)size;
    _inputs = uint32_t(inputs.size());
    _input_declarations.emplace_back(inputs, size);
  }

  void on_outputs(const std::vector<std::string>& outputs,
                  std::string const& size = "") const override {
    (void)size;
    _outputs = uint32_t(outputs.size());
    _output_declarations.emplace_back(outputs, size);
  }

  void on_wires(const std::vector<std::string>& wires,
                std::string const& size = "") const override {
    (void)size;
    _wires = uint32_t(wires.size());
  }

  void on_assign(const std::string& lhs,
                 const std::pair<std::string, bool>& rhs) const override {
    (void)lhs;
    (void)rhs;
    ++_aliases;
  }

  void on_and(const std::string& lhs, const std::pair<std::string, bool>& op1,
              const std::pair<std::string, bool>& op2) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_ands;
  }

  void on_nand(const std::string& lhs, const std::pair<std::string, bool>& op1,
               const std::pair<std::string, bool>& op2) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_nands;
  }

  void on_or(const std::string& lhs, const std::pair<std::string, bool>& op1,
             const std::pair<std::string, bool>& op2) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_ors;
  }

  void on_nor(const std::string& lhs, const std::pair<std::string, bool>& op1,
              const std::pair<std::string, bool>& op2) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_nors;
  }

  void on_xor(const std::string& lhs, const std::pair<std::string, bool>& op1,
              const std::pair<std::string, bool>& op2) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    ++_xors;
  }

  void on_and3(const std::string& lhs, const std::pair<std::string, bool>& op1,
               const std::pair<std::string, bool>& op2,
               const std::pair<std::string, bool>& op3) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    (void)op3;
    ++_ands3;
  }

  void on_or3(const std::string& lhs, const std::pair<std::string, bool>& op1,
              const std::pair<std::string, bool>& op2,
              const std::pair<std::string, bool>& op3) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    (void)op3;
    ++_ors3;
  }

  void on_xor3(const std::string& lhs, const std::pair<std::string, bool>& op1,
               const std::pair<std::string, bool>& op2,
               const std::pair<std::string, bool>& op3) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    (void)op3;
    ++_xors3;
  }

  void on_maj3(const std::string& lhs, const std::pair<std::string, bool>& op1,
               const std::pair<std::string, bool>& op2,
               const std::pair<std::string, bool>& op3) const override {
    (void)lhs;
    (void)op1;
    (void)op2;
    (void)op3;
    ++_maj3;
  }

  void on_endmodule() const override {}

  void on_comment(const std::string& comment) const override {
    (void)comment;
    ++_comments;
  }

  void on_parameter(const std::string& name,
                    const std::string& value) const override {
    _parameter_definitions.emplace_back(name, value);
    ++_parameter;
  }

  void on_module_instantiation(
      std::string const& module_name, std::vector<std::string> const& params,
      std::string const& inst_name,
      std::vector<std::pair<std::string, std::string>> const& args)
      const override {
    ++_instantiations;
    _instantiated_modules.emplace_back(
        module_info{module_name, inst_name, params, args});
  }

  mutable uint32_t _inputs = 0;
  mutable uint32_t _outputs = 0;
  mutable uint32_t _wires = 0;
  mutable uint32_t _aliases = 0;
  mutable uint32_t _ands = 0;
  mutable uint32_t _nands = 0;
  mutable uint32_t _ors = 0;
  mutable uint32_t _nors = 0;
  mutable uint32_t _xors = 0;
  mutable uint32_t _ands3 = 0;
  mutable uint32_t _ors3 = 0;
  mutable uint32_t _xors3 = 0;
  mutable uint32_t _maj3 = 0;
  mutable uint32_t _comments = 0;
  mutable uint32_t _parameter = 0;
  mutable uint32_t _instantiations = 0;

  mutable std::vector<std::pair<std::string, std::string>>
      _parameter_definitions;
  mutable std::vector<std::pair<std::vector<std::string>, std::string>>
      _input_declarations;
  mutable std::vector<std::pair<std::vector<std::string>, std::string>>
      _output_declarations;
  mutable std::vector<module_info> _instantiated_modules;
}; /* simple_verilog_reader */

std::string ReadVerilogFromDataDir(const std::string& v_file_name) {
  // TODO avoid useless string copy x2?
  auto v_path =
      std::filesystem::path(interstellar::data_dir) / "verilog" / v_file_name;
  std::ifstream input_stream_v(v_path, std::ios_base::binary);
  std::ostringstream sstr;
  sstr << input_stream_v.rdbuf();
  return sstr.str();
}

int main() {
  // concat all the data/verilog/*.v into this tmpfile
  // TODO use /tmp + unique name
  // std::FILE* tmpf = std::tmpfile();
  std::string output_name_v = "output.v";

  std::ofstream output_stream_v(output_name_v, std::ios::binary);

  // AddVerilogDefines(&output_stream_v);
  std::string segment2pixels_str =
      ReadVerilogFromDataDir("message-display-segment2pixels.v");
  std::string xorexpand_str = ReadVerilogFromDataDir("xorexpand.v");
  std::string rndswitch_str = ReadVerilogFromDataDir("rndswitch.v");
  std::string display_main_str = ReadVerilogFromDataDir("display-main.v");

  // concat all the .v files
  // WARNING: order matters for verilog compilation !
  std::string output_cat_v = absl::StrCat(segment2pixels_str, xorexpand_str,
                                          rndswitch_str, display_main_str);

  ProcessDefines(&output_cat_v);

  // now we can write the final .v file
  // it SHOULD be readable by yosys and iverilog; CHECK when developping!
  output_stream_v << output_cat_v;
  output_stream_v.flush();

  lorina::text_diagnostics consumer;
  lorina::diagnostic_engine diag(&consumer);
  mockturtle::aig_network aig;
  simple_verilog_reader reader;  // TODO mockturtle::verilog_reader(aig)
  // TODO FIX: it fails when trying to parse at "if ( token != "module" )" whe
  // ntrying to parse "`DEFINE"
  auto const result = lorina::read_verilog(output_name_v, reader, &diag);
  assert(result == lorina::return_code::success);

  auto const cuts = cut_enumeration(aig);
  aig.foreach_node([&](auto node) {
    std::cout << cuts.cuts(aig.node_to_index(node)) << "\n";
  });

  return 0;
}
