#include "verilog_compiler.h"

#include "absl/strings/str_cat.h"
#include "kernel/yosys.h"

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
  // TODO proper logging
  void Error(const std::string &message) {
    std::cout << "FilterErrorStreamBuf : Error : " << message;
    throw std::runtime_error(std::string("FilterErrorStreamBuf : ") + message);
  }

  // TODO proper logging
  void Warning(const std::string &message) {
    std::cout << "FilterErrorStreamBuf : Warning : " << message;
  }
};

/**
 * see Yosys sources
 */
static void yosys_setup() {
  // "if there are already IdString objects then we have a global initialization
  // order bug"
  Yosys::IdString empty_id;
  log_assert(empty_id.index_ == 0);
  // Yosys::IdString::get_reference(empty_id.index_); // WARNING: memory leak?!

  Yosys::Pass::init_register();

  // yosys_design = new RTLIL::Design;

  // yosys_celltypes.setup();

  Yosys::log_push();
}

namespace interstellar {

namespace YosysHelper {

// TODO use low-level yosys functions :
// - add error handling
// - avoid parsing strings(a-la cli : args passed to yosys command via strings)
void CompileVerilog(std::string_view input_v_full_path,
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

  // ~ Yosys::run_pass(read_verilog_cmd);
  Yosys::Pass::call(&yosys_design,
                    absl::StrCat("read_verilog ", input_v_full_path));

  // proc could be needed for segment2pixels, depending on the way/if the 0s are
  // run-length encoded in some way
  // TODO test and bench(including circuit_size and eval speed): techmap; opt;
  // proc; etc (old size 2) techmap; opt;    real    0m2.573s user    0m2.441s
  // techmap;         real    0m2.060s user    0m1.906s
  // ~ Yosys::run_pass("techmap;");
  Yosys::Pass::call(&yosys_design, "techmap;");

  // ~ Yosys::run_pass(write_blif_cmd);
  Yosys::Pass::call(&yosys_design,
                    absl::StrCat("write_blif ", output_blif_full_path));

  // Calling this will prevent subsequent yosys_setup from working(pass_register
  // would be empty)
  // TODO cleanup yosys globals/static
  // Yosys::yosys_shutdown();
}

}  // namespace YosysHelper

}  // namespace interstellar