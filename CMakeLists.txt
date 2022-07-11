cmake_minimum_required(VERSION 3.22)

project(lib_circuits)

################################################################################

include(CMakeDependentOption)

option(interstellar_lib_circuits_BUILD_TESTS "Build tests" ON)
cmake_dependent_option(interstellar_lib_circuits_HAS_CLIS "Build the various testing cli" ON "interstellar_lib_circuits_BUILD_TESTS;" ON)
option(DEP_ABC_COMPILE_FROM_SOURCE "Compile ABC from source instead of using a system-installed one" OFF)
option(DEP_YOSYS_COMPILE_FROM_SOURCE "Compile Yosys from source instead of using a system-installed one" OFF)

################################################################################

include(deps/cmake/compile_flags.cmake)
include(deps/cmake/coverage.cmake)
include(deps/cmake/options.cmake)
include(deps/cmake/sanitizers.cmake)

################################################################################

include(3rd_party/cimg.cmake)
include(3rd_party/abseil.cmake)
include(3rd_party/fmt.cmake)
include(3rd_party/glog.cmake)
include(3rd_party/protobuf.cmake)
include(3rd_party/xxhash.cmake)

include(deps/abc.cmake)
include(deps/yosys.cmake)
include(deps/protos.cmake)

################################################################################

# will embed all the .v,.png,etc.
# (ie basically make them install-ready)
add_subdirectory(data)

# the main sources
add_subdirectory(src)

if(interstellar_lib_circuits_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif(interstellar_lib_circuits_BUILD_TESTS)