cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_policy(SET CMP0135 NEW)

project(lib_circuits)

# ##############################################################################
include(CMakeDependentOption)

option(interstellar_lib_circuits_BUILD_TESTS "Build tests" ON)
option(interstellar_lib_circuits_BUILD_RUST_WRAPPER "Build rust_wrapper in circuit-gen-rs/" OFF)
cmake_dependent_option(interstellar_lib_circuits_HAS_CLIS "Build the various testing cli" ON "interstellar_lib_circuits_BUILD_TESTS;" ON)
option(DEP_ABC_COMPILE_FROM_SOURCE "Compile ABC from source instead of using a system-installed one" OFF)
option(DEP_YOSYS_COMPILE_FROM_SOURCE "Compile Yosys from source instead of using a system-installed one" OFF)

# ##############################################################################
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/compile_flags.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/coverage.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/export_libs.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/options.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/cmake/sanitizers.cmake)

# ##############################################################################
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/cimg.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/abseil.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/fmt.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/3rd_party/glog.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/deps/abc.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/deps/yosys.cmake)

# ##############################################################################
if(interstellar_lib_circuits_BUILD_TESTS)
    include(CTest)
endif(interstellar_lib_circuits_BUILD_TESTS)

# will embed all the .v,.png,etc.
# (ie basically make them install-ready)
add_subdirectory(data)

# ##############################################################################

# the main sources
add_subdirectory(cpp)

if(interstellar_lib_circuits_BUILD_TESTS)
    add_subdirectory(tests)
endif(interstellar_lib_circuits_BUILD_TESTS)

if(interstellar_lib_circuits_BUILD_RUST_WRAPPER)
    add_subdirectory(src)
endif(interstellar_lib_circuits_BUILD_RUST_WRAPPER)