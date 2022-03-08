#include "abc_wrapper.h"

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

class AbcWrapper {
 public:
  AbcWrapper();

  void Execute(std::string_view cmd);

  ~AbcWrapper();

 private:
  // TODO unique_ptr, but the dtor requires Abc_Frame_t as complete type
  Abc_Frame_t *abc_frame_t_;

  // TODO ideally we want ABC to be thread safe, and avoid any lock...
  // inline needs c++17 https://stackoverflow.com/a/62915890/5312991
  inline static absl::Mutex lock_;
  absl::MutexLock yosys_mutexlock_;
};

// template<typename AbcFrameT>
AbcWrapper::AbcWrapper() : yosys_mutexlock_(&AbcWrapper::lock_) {
  // NOT thread safe!
  // "start the ABC framework"
  Abc_Start();

  abc_frame_t_ = Abc_FrameGetGlobalFrame();

  // abc_frame_t_ = Abc_FrameAllocate();
  // "perform initializations"
  // DO NOT CALL if Abc_Start/Abc_FrameGetGlobalFrame was NOT called
  // b/c it depends on the ABC global static "s_GlobalFrame"
  // Abc_FrameInit(abc_frame_t_);
}

// template<typename AbcFrameT>
void AbcWrapper::Execute(std::string_view cmd) {
  // Cmd_CommandExecute(pAbc, (char *)read_cmd.c_str())
  if (Cmd_CommandExecute(abc_frame_t_, cmd.data())) {
    auto msg = absl::StrCat("Cannot execute command : ", cmd);
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }
}

// template<typename AbcFrameT>
AbcWrapper::~AbcWrapper() {
  // stop the ABC framework
  Abc_Stop();
  // TODO replace by
  //"" perform uninitializations"
  // Abc_FrameEnd( pAbc );
  // "stop the framework"
  // Abc_FrameDeallocate( pAbc );

  // DO NOT delete !
  // Abc_FrameGetGlobalFrame returns a static glabal
  // delete abc_frame_t_;
}

namespace interstellar {

namespace ABC {

void Run(std::string_view input_blif_path, std::string_view output_blif_path) {
  AbcWrapper abc_wrapper;

  abc_wrapper.Execute(
      absl::StrCat("read ", interstellar::data_dir, "/verilog/skcd.genlib"));

  abc_wrapper.Execute(absl::StrCat("read ", input_blif_path));

  abc_wrapper.Execute("strash");

  abc_wrapper.Execute("rewrite");

  abc_wrapper.Execute("refactor");

  abc_wrapper.Execute("map -a");

  abc_wrapper.Execute(absl::StrCat("write ", output_blif_path));

  LOG(INFO) << "wrote : " << output_blif_path;
}

}  // namespace ABC

}  // namespace interstellar