#include <filesystem>

#include "absl/strings/str_cat.h"
#include "absl/strings/str_replace.h"
#include "lorina/diagnostics.hpp"
// #include <mockturtle/mockturtle.hpp>
#define LORINA_USE_NEW_PARSER 1
#if LORINA_USE_NEW_PARSER
#include "lorina/verilog/parser.hpp"  // new version
#else
#include "lorina/verilog.hpp"  // apparently uses the OLD version
#endif

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

// TODO for dev/debug?
// https://github.com/hriener/lorina/blob/e05e81df2a8c0317739875795fab00d26a4bc264/test/verilog.cpp

int main() {
  // concat all the data/verilog/*.v into this tmpfile
  // TODO use /tmp + unique name
  // std::FILE* tmpf = std::tmpfile();
  std::string output_name_v = "output.v";
  std::ofstream output_stream_v(output_name_v, std::ios::binary);

  // AddVerilogDefines(&output_stream_v);
  std::string segment2pixels_str = ReadVerilogFromDataDir("segment2pixels.v");
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

#if LORINA_USE_NEW_PARSER
  std::istringstream in(output_cat_v);
  // DO NOT REMOVE! without this lorina's lexer completely fails to handle
  // space(" ") and bundle tokens
  //
  // eg at line "if ( identifier == "module" )" in// lorina/verilog/lexer.hpp
  // identifier is expected to be "module" but without
  // this it is "modulesegment2pixel"
  std::noskipws(in);
  using Lexer = lorina::verilog_lexer<std::istream_iterator<char>>;
  Lexer lexer((std::istream_iterator<char>(in)), std::istream_iterator<char>());

  lorina::verilog_ast_graph ag;
  lorina::verilog_parser parser(lexer, ag, &diag);
  lorina::verilog_ast_graph::ast_or_error ast = parser.consume_module();
  // CHECK(ast);
  assert(ast.valid() && "ast not valid!");
#else
  simple_verilog_reader reader;  // TODO mockturtle::verilog_reader(aig)
  // TODO FIX: it fails when trying to parse at "if ( token != "module" )" whe
  // ntrying to parse "`DEFINE"
  auto const result = lorina::read_verilog(output_name_v, reader, &diag);
  assert(result == lorina::return_code::success);
#endif

  // mockturtle::aig_network aig;
  // auto const cuts = cut_enumeration(aig);
  // aig.foreach_node([&](auto node) {
  //   std::cout << cuts.cuts(aig.node_to_index(node)) << "\n";
  // });

  return 0;
}
