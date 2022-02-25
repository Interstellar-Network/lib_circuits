cmake_minimum_required(VERSION 3.22)

project(lib_circuits)

################################################################################

option(lib_circuits_BUILD_TESTS "Build tests" ON)

################################################################################

include(deps/cmake/compile_flags.cmake)
include(deps/cmake/coverage.cmake)
include(deps/cmake/options.cmake)
include(deps/cmake/sanitizers.cmake)

################################################################################

include(3rd_party/abc.cmake)
include(3rd_party/cimg.cmake)
include(3rd_party/abseil.cmake)
include(3rd_party/fmt.cmake)
include(3rd_party/glog.cmake)
# include(3rd_party/lorina.cmake)  # TODO REMOVE
# include(3rd_party/mockturle.cmake)
include(3rd_party/protobuf.cmake)
include(3rd_party/xxhash.cmake)
include(3rd_party/yosys.cmake) # TODO REMOVE ? if possible: use lorina+mockturtle instead?

include(deps/protos.cmake)

################################################################################

# will embed all the .v,.png,etc.
# (ie basically make them install-ready)
add_subdirectory(data)

# the main sources
add_subdirectory(src)

if (lib_circuits_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif (lib_circuits_BUILD_TESTS)