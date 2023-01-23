cmake_minimum_required(VERSION 3.22)

project(lib_circuits)

# ##############################################################################
include(CMakeDependentOption)

option(interstellar_lib_circuits_BUILD_TESTS "Build tests" ON)
cmake_dependent_option(interstellar_lib_circuits_HAS_CLIS "Build the various testing cli" ON "interstellar_lib_circuits_BUILD_TESTS;" ON)
option(DEP_ABC_COMPILE_FROM_SOURCE "Compile ABC from source instead of using a system-installed one" OFF)
option(DEP_YOSYS_COMPILE_FROM_SOURCE "Compile Yosys from source instead of using a system-installed one" OFF)

# ##############################################################################
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/compile_flags.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/coverage.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/sanitizers.cmake)

# ##############################################################################
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/cimg.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/abseil.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/fmt.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/glog.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/protobuf.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/deps/abc.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/yosys.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/protos.cmake)

# ##############################################################################

# will embed all the .v,.png,etc.
# (ie basically make them install-ready)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/data)

# the main sources
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)

if(interstellar_lib_circuits_BUILD_TESTS)
  enable_testing()
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
endif(interstellar_lib_circuits_BUILD_TESTS)