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

// wrapper for our "lib_server" circuit generator

#pragma once

#include <memory>

#include "rust/cxx.h"

// rust-cxx shared struct
struct ConfigHelper;

/**
 * Write a `.blif.blif` file; this is the C++/Rust border.
 * It will then be parsed from Rust cf `blif_parser.rs`
 */
// TODO not const, but will certainly break Rust's no_std CXX side():
// self: Pin<&mut GenerateDisplaySkcdWrapper>,
//  ^^^ could not find `std` in the list of imported crates
rust::Vec<u_int8_t> GenerateDisplayCircuit(
    uint32_t width, uint32_t height, const rust::Vec<float> &digits_bboxes,
    ConfigHelper &config_helper);

rust::Vec<u_int8_t> GenerateGenericCircuit(rust::Vec<u_int8_t> verilog_buf);
