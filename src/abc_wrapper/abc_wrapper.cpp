#include "abc_wrapper.h"

#include <absl/strings/str_cat.h>
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
};

// template<typename AbcFrameT>
AbcWrapper::AbcWrapper() {
  // TODO replace by cf
  // https://github.com/berkeley-abc/abc/blob/master/src/demo.c "start the
  // framework" s_GlobalFrame = Abc_FrameAllocate(); "perform initializations"
  // Abc_FrameInit( s_GlobalFrame );

  // start the ABC framework
  Abc_Start();

  // Abc_Frame_t *pAbc = Abc_FrameGetGlobalFrame();
  // abc_frame_t_ = std::make_unique<Abc_Frame_t>();
  abc_frame_t_ = Abc_FrameGetGlobalFrame();
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
}

}  // namespace ABC

}  // namespace interstellar