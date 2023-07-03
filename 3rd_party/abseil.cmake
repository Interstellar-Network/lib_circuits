# ##############################################################################
include(FetchContent)

FetchContent_Declare(
  abseil

  # "Abseil recommends users "live-at-head" (update to the latest commit from the master branch as often as possible)."
  # but we use LTS branch for now
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG 20230125.3
)

# avoids a warning:
# A future Abseil release will default ABSL_PROPAGATE_CXX_STD to ON for CMake
# [build]   3.8 and up.  We recommend enabling this option to ensure your project still
# [build]   builds correctly.
option(ABSL_PROPAGATE_CXX_STD
  "Use CMake C++ standard meta features (e.g. cxx_std_11) that propagate to targets that link to Abseil"
  ON) # default to OFF

FetchContent_MakeAvailable(abseil)

target_compile_options(absl_crc32c
  PRIVATE

  # error: ignoring attributes on template argument ‘__m128i’ [-Werror=ignored-attributes]
  -Wno-ignored-attributes
)
