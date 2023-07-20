// Copyright 2022 Nathan Prat

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

use cmake::Config;
use rust_cxx_cmake_bridge::read_cmake_generated;

// NOTE: check git history for a "working" version using shared libs
// It worked locally but was a pain to deploy/package cf "DBUILD_SHARED_LIBS" below

fn main() {
    // BEFORE CMake: that will (among other things) generate rust/cxx.h that
    // is needed to compile src/rust_wrapper.cpp
    // ALTERNATIVELY we could add a git submodule for https://github.com/dtolnay/cxx/tree/master/include
    cxx_build::bridge("src/lib.rs").compile("circuit-gen-rs");

    let mut cmake_config = Config::new(".");
    cmake_config.generator("Ninja");
    // NOTE: SHOULD NOT use shared libs
    // b/c it makes really messy to package/deploy/dockerize
    // Also makes it hard to debug and run tests from just this package while in parent crate.
    // (ie Undefined Reference)
    cmake_config.configure_arg("-DBUILD_SHARED_LIBS=OFF");
    // TODO? it is the default but just in case[Yosys does NOT work with STATIC]
    // https://github.com/YosysHQ/yosys/issues/3241
    cmake_config.configure_arg("-DYOSYS_BUILD_SHARED_LIBS=ON");
    cmake_config.configure_arg("-Dinterstellar_lib_circuits_BUILD_TESTS=OFF");
    cmake_config.configure_arg("-Dinterstellar_lib_circuits_BUILD_RUST_WRAPPER=ON");
    // TODO use IPO/LTO, at least in Release
    cmake_config.build_target("rust_wrapper");
    // without this(default to true) cmake is run every time, and for some reason this thrashes the build dir
    // which causes to recompile from scratch every time(for eg a simple comment added in lib.rs)
    cmake_config.always_configure(false); // TODO always_configure

    // For this to be seen: eg `RUST_LOG=cmake=trace cargo test > cmake_trace.log 2>&1`
    // cmake_config.configure_arg("--trace");
    // cmake_config.very_verbose(true);

    let rust_wrapper = cmake_config.build();

    // rust_wrapper.display() = /home/.../api_circuits/target/debug/build/circuit-gen-rs-XXX/out
    // but the final lib we want(eg librust_wrapper.a) is below in build/src/
    // TODO remove? this is done as part of the loop below
    println!(
        "cargo:rustc-link-search=native={}/build/src/",
        rust_wrapper.display()
    );
    println!("cargo:rustc-link-lib=static=rust_wrapper");

    //     # Else:
    // # error: linking with `clang` failed: exit status: 1
    // # |
    // # = note: LC_ALL="C" PATH="/home/pratn/.rustup/toolchains/stable-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/bin:/home/pratn/.vscode-server/bin/695af097c7bd098fbf017ce3ac85e09bbc5dda06/bin/remote-cli:/home/pratn/.cargo/bin:/usr/local/sbin:/usr/local/bin:/usr/bin:/usr/lib/jvm/default/bin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl:/home/pratn/Android/Sdk/platform-tools/:/opt/android-studio/bin/:/home/pratn/Android/Sdk/platform-tools/:/opt/android-studio/bin/" VSLANG="1033" "clang" "-m64" "/tmp/rustcQLhYoq/symbols.o" "/home/pratn/workspace/interstellar/api_circuits/target/debug/deps/
    // # ...
    // # stable-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib/librustc_std_workspace_core-207f06e41d9603af.rlib" "/home/pratn/.rustup/toolchains/stable-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib/libcore-7e2768e66e984e85.rlib" "/home/pratn/.rustup/toolchains/stable-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib/libcompiler_builtins-957b4aff41f8cd46.rlib" "-Wl,-Bdynamic" "-labc" "-lpng" "-lyosys" "-lstdc++" "-lgcc_s" "-lutil" "-lrt" "-lpthread" "-lm" "-ldl" "-lc" "-Wl,--eh-frame-hdr" "-Wl,-z,noexecstack" "-L" "/home/pratn/.rustup/toolchains/stable-x86_64-unknown-linux-gnu/lib/rustlib/x86_64-unknown-linux-gnu/lib" "-o" "/home/pratn/workspace/interstellar/api_circuits/target/debug/deps/api_circuits-49104f341b0c5b61" "-Wl,--gc-sections" "-pie" "-Wl,-z,relro,-z,now" "-nodefaultlibs" "-fuse-ld=/usr/bin/mold" "-Wl,--no-rosegment"
    // # = note: mold: error: undefined symbol: _Ux86_64_getcontext
    // # >>> referenced by utilities.cc
    // # >>>               /home/pratn/workspace/interstellar/api_circuits/target/debug/deps/liblib_circuits_wrapper-4178f015683f00fb.rlib(utilities.cc.o):(google::GetStackTrace(void**, int, int))
    // # mold: error: undefined symbol: _ULx86_64_init_local
    // # >>> referenced by utilities.cc
    // # >>>               /home/pratn/workspace/interstellar/api_circuits/target/debug/deps/liblib_circuits_wrapper-4178f015683f00fb.rlib(utilities.cc.o):(google::GetStackTrace(void**, int, int))
    // # mold: error: undefined symbol: _ULx86_64_get_reg
    // # >>> referenced by utilities.cc
    // # >>>               /home/pratn/workspace/interstellar/api_circuits/target/debug/deps/liblib_circuits_wrapper-4178f015683f00fb.rlib(utilities.cc.o):(google::GetStackTrace(void**, int, int))
    // # mold: error: undefined symbol: _ULx86_64_step
    // # >>> referenced by utilities.cc
    // # >>>               /home/pratn/workspace/interstellar/api_circuits/target/debug/deps/liblib_circuits_wrapper-4178f015683f00fb.rlib(utilities.cc.o):(google::GetStackTrace(void**, int, int))
    // # clang-15: error: linker command failed with exit code 1 (use -v to see invocation)
    // #
    // # NOTE: only needed when compiling from `api_circuits` b/c static linking from Rust
    //
    // TRIED to add eg "set_target_properties(unwind::unwind PROPERTIES IMPORTED_GLOBAL TRUE)" in lib_circuits_wrapper/deps/lib_circuits/3rd_party/glog.cmake
    // with "list(APPEND CMAKE_MODULE_PATH ${glog_SOURCE_DIR}/cmake)" and "find_package(Unwind)"
    // cf https://github.com/google/glog/blob/3a0d4d22c5ae0b9a2216988411cfa6bf860cc372/CMakeLists.txt#L93
    // but no success...
    println!("cargo:rustc-link-lib=unwind");

    // target/debug/build/circuit-gen-rs-XXX/out/build/src/cmake_generated_libs
    let path = format!("/{}/build/src/cmake_generated_libs", rust_wrapper.display());
    let cmake_generated_libs_str = std::fs::read_to_string(path).expect("path: {path}");

    read_cmake_generated(&cmake_generated_libs_str);

    // TODO get the system libs using ldd?
    // println!("cargo:rustc-link-lib=readline");

    // But careful, we MUST recompile if the .cpp, the .h or any included .h is modified
    // and using rerun-if-changed=src/lib.rs make it NOT do that
    println!("cargo:rerun-if-changed=src/");
    println!("cargo:rerun-if-changed=CMakeLists.txt");
    println!("cargo:rerun-if-changed=../src/");
    println!("cargo:rerun-if-changed=../CMakeLists.txt");
    println!("cargo:rerun-if-changed=build.rs");
}
