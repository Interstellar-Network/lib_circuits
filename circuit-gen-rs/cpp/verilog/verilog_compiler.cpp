// Copyright 2022 Nathan Prat

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "verilog_compiler.h"

#include <absl/base/call_once.h>
#include <absl/strings/match.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <absl/synchronization/mutex.h>
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
class FilterErrorStreamBuf : public std::stringbuf {
 protected:
  std::streamsize xsputn(const char *s, std::streamsize n) override {
    if (absl::StrContainsIgnoreCase(s, "error")) {
      Error(s);
    }

    if (absl::StrContainsIgnoreCase(s, "warning")) {
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
  // Setting yosys_share_dirname avoid having to package/install yosys
  // system-wide. WARNING: this will FAIL if this dir DOES NOT contain any
  // required file For now we only need techmap.v
  // TODO
  // Yosys::yosys_share_dirname =
  //     "/home/pratn/Documents/interstellar/lib_circuits/build/_deps/"
  //     "yosys_dl-src/share/";
  Yosys::yosys_share_dirname = absl::StrCat(interstellar::data_dir, "/yosys/");

#if 1
  Yosys::yosys_setup();
#else
  // "if there are already IdString objects then we have a global initialization
  // order bug"
  Yosys::IdString empty_id;
  log_assert(empty_id.index_ == 0);
  // Yosys::IdString::get_reference(empty_id.index_); // WARNING: memory leak?!

  // Not needed b/c we set Yosys::yosys_share_dirname
  // init_share_dirname();
  // Not needed b/c we do not use yosys-abc
  // init_abc_executable_name();

  // TODO?
  // #define X(_id) RTLIL::ID::_id = "\\" #_id;
  // #include "kernel/constids.inc"
  // #undef X

  Yosys::Pass::init_register();

  // Not needed b/c we Yosys::Pass::call instead of the high-level API
  // that way we avoid Yosys's global vars
  // yosys_design = new RTLIL::Design;

  // NOT EXPOSED
  // Yosys::yosys_celltypes.setup();

  Yosys::log_push();
#endif
}

namespace interstellar {

namespace VerilogHelper {

ABSL_CONST_INIT absl::Mutex yosys_mutex(absl::kConstInit);
// "is safe to use as a namespace-scoped global variable"
absl::once_flag yosys_setup_flag;

// TODO use low-level yosys functions :
// - add error handling
// - avoid parsing strings(a-la cli : args passed to yosys command via strings)
void CompileVerilog(const std::vector<std::string_view> &inputs_v_full_paths,
                    std::string_view output_blif_full_path) {
  // DO NOT REMOVE
  // Yosys is NOT thread safe
  // NEITHER the setup() NOR the call()
  absl::MutexLock yosys_lock(&yosys_mutex);

  absl::call_once(yosys_setup_flag, yosys_setup);
  // yosys_setup();

  // Make sure the exceptions thrown are propagated
  // NOTE: for some reason calling this in `yosys_setup` or new function
  //   fail when called from Rust; the C++ tests are OK though...
  //   Yes, even when using "static", etc
  // https    :  //
  // stackoverflow.com/questions/34364968/c-stl-streambuf-exception-handling
  FilterErrorStreamBuf filtered_streambuf;
  std::ostream my_ostream_filtered(&filtered_streambuf);
  my_ostream_filtered.exceptions(std::ios_base::badbit);
  // IMPORTANT MUST clear() else it logs everywhere; which means calling
  // "Yosys::Pass::call" after a failure will throw an exception
  Yosys::log_streams.clear();
  Yosys::log_streams.push_back(&my_ostream_filtered);
  // DEBUG: log everything! TOREMOVE, DO NOT COMMIT/PUSH
  // Yosys::log_streams.push_back(&std::cout);
  Yosys::log_error_stderr = true;

  Yosys::RTLIL::Design yosys_design;

  // TODO? we could check if the files exist, are readable, etc BEFORE giving
  // them to Yosys to have cleaner error handling

  // TODO use Yosys::run_pass(read_verilog_cmd) everywhere?
  Yosys::Pass::call(
      &yosys_design,
      absl::StrCat("read_verilog ", absl::StrJoin(inputs_v_full_paths, " "))); //add -sv for system verilog backward-compatible 

  // TODO?
  // https://stackoverflow.com/questions/31434380/what-is-a-good-template-yosys-synthesis-script
  // Yosys::Pass::call(&yosys_design, "synth"); -> this is sort of the commands
  // below: CHECK: the default with "./yosys_exe" > "help synth"
  Yosys::Pass::call(&yosys_design, "hierarchy -check -auto-top");
  Yosys::Pass::call(&yosys_design, "proc");
  Yosys::Pass::call(&yosys_design, "flatten");
  Yosys::Pass::call(&yosys_design, "opt_expr");
  Yosys::Pass::call(&yosys_design, "opt_clean");
  Yosys::Pass::call(&yosys_design, "check");
  Yosys::Pass::call(&yosys_design, "opt -nodffe -nosdff");
  Yosys::Pass::call(&yosys_design, "fsm");
  Yosys::Pass::call(&yosys_design, "opt");
  Yosys::Pass::call(&yosys_design, "wreduce");
  Yosys::Pass::call(&yosys_design, "peepopt");
  Yosys::Pass::call(&yosys_design, "opt_clean");
  // Yosys::Pass::call(&yosys_design, "techmap -map +/cmp2lut.v -map
  // +/cmp2lcu.v");
  Yosys::Pass::call(&yosys_design, "alumacc");
  Yosys::Pass::call(&yosys_design, "share");
  Yosys::Pass::call(&yosys_design, "opt");
  Yosys::Pass::call(&yosys_design, "memory -nomap");
  Yosys::Pass::call(&yosys_design, "opt_clean");

  Yosys::Pass::call(&yosys_design, "opt -fast -full");
  Yosys::Pass::call(&yosys_design, "memory_map");
  Yosys::Pass::call(&yosys_design, "opt -full");
  Yosys::Pass::call(&yosys_design, "techmap");
  // Yosys::Pass::call(&yosys_design, "techmap -map +/gate2lut.v");
  // Yosys::Pass::call(&yosys_design, "clean; opt_lut");
  // Yosys::Pass::call(&yosys_design, "flowmap -maxlut K");
  Yosys::Pass::call(&yosys_design, "opt -fast");

  // TODO? with or without: -assert?
  Yosys::Pass::call(&yosys_design, "hierarchy -check");
  Yosys::Pass::call(&yosys_design, "stat");
  Yosys::Pass::call(&yosys_design, "check");
  // Yosys::Pass::call(&yosys_design, "synth -noabc");
  // Yosys::Pass::call(&yosys_design, "abc");
  // Yosys::Pass::call(&yosys_design, "clean");
  // Yosys::Pass::call(&yosys_design, "opt");

  // TODO abc ?
  // "Note that this is a logic optimization pass within Yosys that is calling
  // ABC internally. This is not going to "run ABC on your design". It will
  // instead run ABC on logic snippets extracted from your design. You will not
  // get any useful output when passing an ABC script that writes a file.
  // Instead write your full design as BLIF file with write_blif and the load
  // that into ABC externally if you want to use ABC to convert your design into
  // another format."

  // proc could be needed for segments2pixels, depending on the way/if the 0s
  // are run-length encoded in some way
  // TODO test and bench(including circuit_size and eval speed): techmap; opt;
  // proc; etc (old size 2) techmap; opt;    real    0m2.573s user    0m2.441s
  // techmap;         real    0m2.060s user    0m1.906s

  Yosys::Pass::call(
      &yosys_design,
      // https://www.reddit.com/r/yosys/comments/8vvflm/why_are_some_of_the_wires_nondriven_in_this/
      absl::StrCat("write_blif -noalias ", output_blif_full_path));

  // Calling this will prevent subsequent yosys_setup from working(pass_register
  // would be empty)
  // TODO cleanup yosys globals/static
  // Yosys::yosys_shutdown();
}

}  // namespace VerilogHelper

}  // namespace interstellar
