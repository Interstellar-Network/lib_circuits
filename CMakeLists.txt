cmake_minimum_required(VERSION 3.22)

project(lib_circuits)

################################################################################

option(lib_circuits_BUILD_TESTS "Build tests" ON)

################################################################################

# If not ON, all "add_library" will default to STATIC
# TODO test with shared libs, and update build.rs if it works!
option(BUILD_SHARED_LIBS "Build shared libraries (.so)." OFF)

# dev option: sanitizer
# If ON, you should build with at least O1
# TODO? memory sanitizer, but that requires recompling libc++ & libc++abi
option(USE_ADDRESS_SANITIZER "Build with fsanitize: address, leak" OFF)
option(USE_THREAD_SANITIZER "Build with fsanitize: thread" OFF)
option(USE_MEMORY_SANITIZER "Build with fsanitize: memory" OFF)

# c++17: needed for string_view
# c++20: why not?
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force PIC on all target
# (it's needed for most anyway since we are building a shared lib for the Python wrapper)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# enable LTO because why not
# also pretty much a prerequisite for Rust FFI cf https://github.com/XiangpengHao/cxx-cmake-example
# NOTE: behind an option b/c that REALLY slows down the linking (even in Debug)
option(ENABLE_LTO "Enable cross language linking time optimization" OFF)
if(ENABLE_LTO)
  if(supported)
      message(STATUS "IPO / LTO enabled")
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  else()
      message(STATUS "IPO / LTO not supported: <${error}>")
  endif()
endif()

################################################################################
# set the compile flags BEFORE adding any internal lib/exe
# [but possibly AFTER any external 3rd party deps b/c they MAY NOT compile with
# all warnings]

add_compile_options(
    -march=native

    -Wall -Wextra -Werror

    -fstack-protector-strong

    # without this when debugging we can not see the stack values
    $<$<CONFIG:Debug>:-fno-limit-debug-info>
)

################################################################################

include(3rd_party/mockturle.cmake)
include(3rd_party/abseil.cmake)

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