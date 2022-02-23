#include "verilog_compiler.h"

#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <glog/logging.h>
#include <kernel/yosys.h>

#include "resources.h"

/**
 * Yosys has no erro handling, or exception...
 * So we intercept the yosys logs and check if there are any "warning" or
 * "error".
 *
 * Alternatively we could call Yosys in a thread and then check the return/exit
 * code.
 */
class FilterErrorStreamBuf : public std::streambuf {
 protected:
  std::streamsize xsputn(const char *s, std::streamsize n) override {
    if (strstr(s, "ERROR") != nullptr) {
      Error(s);
    } else if (strstr(s, "Error") != nullptr) {
      Error(s);
    } else if (strstr(s, "error") != nullptr) {
      Error(s);
    }

    if (strstr(s, "WARNING") != nullptr) {
      Warning(s);
    } else if (strstr(s, "Warning") != nullptr) {
      Warning(s);
    } else if (strstr(s, "warning") != nullptr) {
      Warning(s);
    }

    return n;
  };

 private:
  void Error(const std::string &message) {
    auto msg = absl::StrCat("FilterErrorStreamBuf : Error : ", message);
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }

  void Warning(const std::string &message) {
    LOG(WARNING) << absl::StrCat("FilterErrorStreamBuf : Warning : ", message);
  }
};

/**
 * see Yosys sources
 */
// TODO(yosys) remove call to Yosys::yosys_setup and do only what is strictly
// needed
static void yosys_setup() {
  // TODO provide the techmap.v ourself that way the one in /usr/share/yosys
  // will not be used ? FAIL: see below "Found control character or space"
  // TODO at least avoid harcoding the path(use resources.h)
  // Yosys::yosys_share_dirname =
  //     absl::StrCat(interstellar::data_dir, "/verilog/");
  //
  // 7.1. Executing Verilog-2005 frontend:
  // .../build/_deps/yosys_dl-src/share/techmap.v
  // Parsing Verilog input from `.../build/_deps/yosys_dl-src/share/techmap.v'
  // to AST representation
  //
  // MUST be before init_share_dirname(called by Yosys::yosys_setup)
  Yosys::yosys_share_dirname =
      "/home/pratn/Documents/interstellar/lib_circuits/build/_deps/"
      "yosys_dl-src/share/";

#if 1
  Yosys::yosys_setup();
#else
  // "if there are already IdString objects then we have a global initialization
  // order bug"
  Yosys::IdString empty_id;
  log_assert(empty_id.index_ == 0);
  // Yosys::IdString::get_reference(empty_id.index_); // WARNING: memory leak?!

  Yosys::Pass::init_register();

  // yosys_design = new RTLIL::Design;

  // yosys_celltypes.setup();

  Yosys::log_push();
#endif
}

namespace interstellar {

namespace VerilogHelper {

// TODO use low-level yosys functions :
// - add error handling
// - avoid parsing strings(a-la cli : args passed to yosys command via strings)
void CompileVerilog(const std::vector<std::string_view> &inputs_v_full_paths,
                    std::string_view output_blif_full_path) {
  static bool is_setup = false;

  if (!is_setup) {
    static FilterErrorStreamBuf filtered_streambuf;
    static std::ostream yosys_log_stream(&filtered_streambuf);
    Yosys::log_streams.push_back(&yosys_log_stream);
    Yosys::log_error_stderr = true;

    // Custom version, still needed for the Pass register
    // TODO remove and add custom Pass::call
    yosys_setup();

    is_setup = true;
  } else {
    // Not the first time, nothing to setup
  }

  // TODO remove the Pass and use the Frontend directly eg "Yosys::Pass"?
  // Yosys::Pass::init_register(); // normally called by yosys_setup

  Yosys::RTLIL::Design yosys_design;

  // TODO? we could check if the files exist, are readable, etc BEFORE giving
  // them to Yosys to have cleaner error handling

  // TODO use Yosys::run_pass(read_verilog_cmd) everywhere?
  Yosys::Pass::call(
      &yosys_design,
      absl::StrCat("read_verilog ", absl::StrJoin(inputs_v_full_paths, " ")));

  // TODO?
  // https://stackoverflow.com/questions/31434380/what-is-a-good-template-yosys-synthesis-script
  // Yosys::Pass::call(&yosys_design, "proc");
  // Yosys::Pass::call(&yosys_design, "opt");
  // Yosys::Pass::call(&yosys_design, "synth");
  // Yosys::Pass::call(&yosys_design, "opt");
  // Yosys::Pass::call(&yosys_design, "techmap");
  // Yosys::Pass::call(&yosys_design, "opt");

  Yosys::Pass::call(&yosys_design, "synth -noabc");
  // Yosys::Pass::call(&yosys_design, "abc");
  Yosys::Pass::call(&yosys_design, "clean");

  // TODO abc ?
  // "Note that this is a logic optimization pass within Yosys that is calling
  // ABC internally. This is not going to "run ABC on your design". It will
  // instead run ABC on logic snippets extracted from your design. You will not
  // get any useful output when passing an ABC script that writes a file.
  // Instead write your full design as BLIF file with write_blif and the load
  // that into ABC externally if you want to use ABC to convert your design into
  // another format."

  // proc could be needed for segment2pixels, depending on the way/if the 0s are
  // run-length encoded in some way
  // TODO test and bench(including circuit_size and eval speed): techmap; opt;
  // proc; etc (old size 2) techmap; opt;    real    0m2.573s user    0m2.441s
  // techmap;         real    0m2.060s user    0m1.906s

  Yosys::Pass::call(&yosys_design,
                    absl::StrCat("write_blif ", output_blif_full_path));

  // Calling this will prevent subsequent yosys_setup from working(pass_register
  // would be empty)
  // TODO cleanup yosys globals/static
  // Yosys::yosys_shutdown();
}

}  // namespace VerilogHelper

}  // namespace interstellar