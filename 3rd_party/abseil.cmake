include(${CMAKE_CURRENT_LIST_DIR}/_conan.cmake)

################################################################################

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES abseil/20211102.0
                      GENERATORS cmake_find_package)

# NO!
# FAIL:
# ERROR: Missing prebuilt package for...
# - We DO NOT care if the package was built with gcc even if locally we are using clang
# - We WANT to always use Release libs even when building Debug locally(SHOULD be configurable)
# conan_cmake_autodetect(settings)

message(WARNING "settings : ${settings}")

conan_cmake_install(PATH_OR_REFERENCE .
                    REMOTE conancenter
                    # SETTINGS ${settings}
                    # IMPORTANT, b/c it ends up impacting ABSL_OPTION_USE_STD_STRING_VIEW
                    # which is 0 without this, which means it uses absl internal string_view
                    # which means there is NO conversion from str::string_view -> absl::string_view
                    # and that breaks a lot of function
                    SETTINGS compiler.cppstd=17
                    # TODO find a prebuilt binary with correct ABSL_OPTION_USE_STD_STRING_VIEW 0
                    BUILD missing
                    #
                    # OK ONLY IF with find->replace ABSL_OPTION_USE_STD_STRING_VIEW 1
                    # SETTINGS compiler=clang
                    # SETTINGS compiler.version=13
                    # libcxx: [libstdc++, libc++]
                    # SETTINGS compiler.libcxx=libc++
                    #
                    # That config defaults to "#define ABSL_OPTION_USE_STD_STRING_VIEW 1"
                    # but lots of undefined reference???
                    # SETTINGS compiler=gcc
                    # SETTINGS compiler.version=11
                    # SETTINGS compiler.libcxx=libstdc++11
)

# cf build/Findabsl.cmake for the vars
find_package(absl REQUIRED)

return()

################################################################################

include(FetchContent)

FetchContent_Declare(
    abseil
    # "Abseil recommends users "live-at-head" (update to the latest commit from the master branch as often as possible)."
    # Commits on Feb 17, 2022
    GIT_REPOSITORY  https://github.com/abseil/abseil-cpp.git
    GIT_TAG     7f850b3167fb38e6b4a9ce1824e6fabd733b5d62
)

# avoids a warning:
# A future Abseil release will default ABSL_PROPAGATE_CXX_STD to ON for CMake
# [build]   3.8 and up.  We recommend enabling this option to ensure your project still
# [build]   builds correctly.
option(ABSL_PROPAGATE_CXX_STD
  "Use CMake C++ standard meta features (e.g. cxx_std_11) that propagate to targets that link to Abseil"
  ON) # default to OFF

FetchContent_MakeAvailable(abseil)
