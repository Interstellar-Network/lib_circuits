## Dev

If you want the "quick and easy" setup like in CI:

- install Yosys and ABC, cf .github/workflows/cpp.yml for details
  - TODO(CHECK possibly not-up-to-date) Else: you CAN set CMake options to compile ABC and Yosys from source.

NOTE: this project is Rust first, C++ second but for "ease of dev" it is still structured to
be compiled using the classic `mkdir build && cd build && cmake ../circuit-gen-rs/` which is why there is a `CMakeLists.txt` in this folder.

## Rust wrapper: circuit-gen-rs/

Thin Rust wrapper around `lib_circuits` to easily call it from `pallet-ocw-circuits`.

IMPORTANT: this NOT meant to be compiled with eg `cmake`; It is meant to be compiled via `cargo build`.
You can still use the example command in [## Dev] to check the base parts in C++.

NOTE: originally in repo `api_circuits`

## TODO

- when generating the circuits(.v and/or .skcd) we SHOULD also pacakge(serialize) what is needed for the display shader
eg is it a 2*1 render(message), a 1*10(pinpad), other?

- TODO(rust) add CI
- TODO(rust) CI: run eg `cargo +nightly-2023-04-15 no-std-check --no-default-features --features=alloc,tikv-jemallocator,panic`