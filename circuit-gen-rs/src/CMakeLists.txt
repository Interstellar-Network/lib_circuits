# ###############################################################################

# IMPORTANT: this file is generated/symlinked by `cxx` crate's `build.rs`
# so it ONLY exists when compiling using cargo, not when using cmake directly!
# For reference:
# - eg CMAKE_CURRENT_BINARY_DIR = ./target/debug/build/circuit-gen-rs-dc4a41e11f701140/out/build/src
# - cxx.h PATH                  = ./target/debug/build/circuit-gen-rs-dc4a41e11f701140/out/cxxbridge/include/rust/cxx.h
set(MY_RUST_CXX_DIR "${CMAKE_CURRENT_BINARY_DIR}/../../cxxbridge/include/")
message(INFO " CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}, MY_RUST_CXX_DIR: ${MY_RUST_CXX_DIR}")
find_path(RUST_CXX_INCLUDE_DIR rust/cxx.h
    PATHS "${MY_RUST_CXX_DIR}"
    REQUIRED
)

message(INFO "RUST_CXX_INCLUDE_DIR: ${RUST_CXX_INCLUDE_DIR}")

# mostly to integrate/include rust/cxx.h
add_library(rust_cxx INTERFACE)

target_include_directories(rust_cxx
    INTERFACE
    ${RUST_CXX_INCLUDE_DIR}

    # cf CREATE_LINK below
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

# NOTE: for shared struct, at the top of of the generated "lib.rs.h"
# there is #include "circuit-gen-rs/src/rust_wrapper.h"
# and it MUST point to the file "rust_wrapper.h"
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/circuit-gen-rs/src")
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/rust_wrapper.h" "${CMAKE_CURRENT_BINARY_DIR}/include/circuit-gen-rs/src/rust_wrapper.h" SYMBOLIC)

# ###############################################################################
add_library(rust_wrapper STATIC
    rust_wrapper.cpp

    # that we have a proper dependency on CREATE_LINK
    "${CMAKE_CURRENT_BINARY_DIR}/include/circuit-gen-rs/src/rust_wrapper.h"
)

target_include_directories(rust_wrapper
    PRIVATE

    # for #include "circuit-gen-rs/src/rust_wrapper.h"
    ${PROJECT_BINARY_DIR}/include
)

target_link_libraries(rust_wrapper
    PUBLIC
    rust_cxx

    circuit_lib
)

export_all_target_libs(rust_wrapper)
