#include "abc_wrapper.h"

#include <absl/base/call_once.h>
#include <absl/strings/str_cat.h>
#include <absl/synchronization/mutex.h>
#include <glog/logging.h>

// clang-format off
// clang-format would reorder those includes, but main.h must be first
#include <base/main/main.h>  // abc // NOLINT
#include <base/main/abcapis.h> // NOLINT
#include <base/main/mainInt.h> // NOLINT
// clang-format on

#include "resources.h"

namespace {

void AbcExecute(Abc_Frame_t *abc_frame, std::string_view cmd) {
  if (Cmd_CommandExecute(abc_frame, cmd.data())) {
    auto msg = absl::StrCat("Cannot execute command : ", cmd);
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
}

}  // anonymous namespace

namespace interstellar {

namespace ABC {

ABSL_CONST_INIT absl::Mutex abc_mutex(absl::kConstInit);
// "is safe to use as a namespace-scoped global variable"
absl::once_flag abc_start_flag;

void Run(std::string_view input_blif_path, std::string_view output_blif_path) {
  // TODO ideally we want ABC to be thread safe, and avoid any lock...
  absl::MutexLock abc_lock(&abc_mutex);

  // NOT thread safe!
  // "start the ABC framework"
  // NOTE: also quite slow, same as "Abc_Stop"
  // TODO replace by Abc_FrameAllocate+Abc_FrameInit and remove lock?
  // Abc_Start();
  absl::call_once(abc_start_flag, Abc_Start);

  Abc_Frame_t *abc_frame = Abc_FrameGetGlobalFrame();

  AbcExecute(abc_frame, absl::StrCat("read ", interstellar::data_dir,
                                     "/verilog/skcd.genlib"));

  AbcExecute(abc_frame, absl::StrCat("read ", input_blif_path));

  AbcExecute(abc_frame, "strash");

  AbcExecute(abc_frame, "rewrite");

  AbcExecute(abc_frame, "refactor");

  AbcExecute(abc_frame, "map -a");

  AbcExecute(abc_frame, absl::StrCat("write ", output_blif_path));

  LOG(INFO) << "wrote : " << output_blif_path;

  // TODO this probably leaks?
  // NOTE: this is quite slow
  // stop the ABC framework
  // Abc_Stop();
}

}  // namespace ABC

}  // namespace interstellar