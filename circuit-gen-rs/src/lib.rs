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

// #![no_std]
// https://github.com/substrate-developer-hub/substrate-module-template/blob/master/HOWTO.md#forgetting-cfg_attr-for-no_std
#![cfg_attr(not(feature = "std"), no_std)]
#![deny(elided_lifetimes_in_paths)]
#![warn(clippy::suspicious)]
#![warn(clippy::complexity)]
#![warn(clippy::perf)]
#![warn(clippy::style)]
#![warn(clippy::pedantic)]
#![warn(clippy::expect_used)]
#![warn(clippy::panic)]
#![warn(clippy::unwrap_used)]

#[cfg(feature = "alloc")]
extern crate alloc;

#[cfg(feature = "alloc")]
use alloc::string::ToString;
#[cfg(feature = "alloc")]
use alloc::vec::Vec;

use circuit_types_rs::Circuit;

// re-export for ease-of-use in pallets
pub use circuit_types_rs::*;

#[cfg(test)]
#[macro_use]
extern crate std;

#[cfg(feature = "panic")]
#[panic_handler]
fn panic(_panic: &core::panic::PanicInfo<'_>) -> ! {
    loop {}
}

#[cfg(feature = "tikv-jemallocator")]
#[global_allocator]
static GLOBAL: tikv_jemallocator::Jemalloc = tikv_jemallocator::Jemalloc;

/// Generate a new Circuit("display" variant)
///
/// Step 1: called from pallet-ocw-circuits
///
/// # Errors
///
/// - if the buffer returned from C++ is not a valid ".blif.blif" file
/// - NOTE: the C++ can error-out for various reasons, and these errors WILL NOT be exposed on Rust side!
///
pub fn generate_display_circuit(
    width: u32,
    height: u32,
    digits_bboxes: &Vec<f32>,
) -> Result<Circuit, CircuitParserError> {
    let mut config_helper = ffi::ConfigHelper::default();
    let raw_buf = ffi::GenerateDisplayCircuit(width, height, digits_bboxes, &mut config_helper)
        .map_err(|err| CircuitParserError::CppException {
            msg: err.to_string(),
        })?;

    Circuit::new_display_from_blif_buffer(
        &raw_buf,
        config_helper.has_watermark,
        config_helper.width,
        config_helper.height,
        config_helper.nb_segments,
        config_helper.rndsize,
    )
}

/// Create a new Circuit by compiling a generic Verilog (buffer)
///
/// This is used by pallet-ocw-circuits, generic variant
///
/// NOTE: the buffer is written to a file on the C++ side
/// We do it in C++ b/c the crate "tempfile" does NOT work in `no_std`.
/// Also there is no `core::io::write`...
///
/// # Errors
///
/// - if the buffer returned from C++ is not a valid ".blif.blif" file
/// - NOTE: the C++ can error-out for various reasons, and these errors WILL NOT be exposed on Rust side!
///   eg: the `verilog_buf` is not valid Verilog, etc
///
pub fn new_from_verilog(verilog_buf: &[u8]) -> Result<Circuit, CircuitParserError> {
    let raw_buf = ffi::GenerateGenericCircuit(verilog_buf.to_vec()).map_err(|err| {
        CircuitParserError::CppException {
            msg: err.to_string(),
        }
    })?;

    Circuit::new_generic_from_blif_buffer(&raw_buf)
}

#[cxx::bridge]
mod ffi {
    /// Basic wrapper to easily pass around the parameters needed for circuit-gen-rs/src/circuit/config.rs
    /// `CircuitDisplayConfig::new`
    ///
    #[derive(Debug, Default)]
    pub struct ConfigHelper {
        has_watermark: bool,
        width: u32,
        height: u32,
        nb_segments: u32,
        rndsize: u32,
    }

    unsafe extern "C++" {
        include!("circuit-gen-rs/src/rust_wrapper.h");

        /// * `digits_bboxes` - a list of BBox, one per digit
        /// passed as
        /// (lower_left_corner.x, lower_left_corner.y,
        /// upper_right_corner.x, upper_right_corner.y)
        ///
        /// DO NOT return a cxx:String b/c those MUST contain valid UTF8/16
        /// and the returned buffer DO NOT (they are protobuf bin)
        /// Same with return: &str, String
        /// terminate called after throwing an instance of 'std::invalid_argument'
        ///   what():  data for rust::Str is not utf-8
        ///
        /// return:
        /// * `Vec<u8>` ie raw buffer containing a .blif.blif
        /// * config: corresponding to the skcd.pb.bin's config field
        fn GenerateDisplayCircuit(
            width: u32,
            height: u32,
            digits_bboxes: &Vec<f32>,
            config_helper: &mut ConfigHelper,
        ) -> Result<Vec<u8>>;

        ///
        /// return:
        /// * `Vec<u8>` ie raw buffer containing a .blif.blif
        ///
        fn GenerateGenericCircuit(verilog_buf: Vec<u8>) -> Result<Vec<u8>>;
    }
}

#[cfg(test)]
mod tests {
    use crate::ffi;

    #[test]
    fn test_ffi_generate_display_skcd_low_res() {
        let width = 120;
        let height = 52;

        let digits_bboxes = vec![
            // first digit bbox --------------------------------------------
            0.25_f32, 0.1_f32, 0.45_f32, 0.9_f32,
            // second digit bbox -------------------------------------------
            0.55_f32, 0.1_f32, 0.75_f32, 0.9_f32,
        ];

        let mut config_helper = ffi::ConfigHelper::default();
        let raw_buf =
            ffi::GenerateDisplayCircuit(width, height, &digits_bboxes, &mut config_helper).unwrap();

        let buf = remove_first_line_from_buf(raw_buf);
        let ref_buf = include_bytes!(concat!(
            env!("CARGO_MANIFEST_DIR"),
            "/../circuit-gen-rs/tests/data/result_display_message_120x52_2digits.blif.blif"
        ));

        assert_eq!(buf, ref_buf, "failed {buf:#?} vs {ref_buf:#?}");
        assert_eq!(config_helper.width, width);
        assert_eq!(config_helper.height, height);
        assert_eq!(config_helper.has_watermark, true);
        assert_eq!(config_helper.nb_segments, 14);
        assert_eq!(config_helper.rndsize, 14 * 10);
    }

    #[test]
    fn test_generate_generic_full_adder() {
        let raw_buf = ffi::GenerateGenericCircuit(
            include_bytes!(concat!(env!("CARGO_MANIFEST_DIR"), "/data/verilog/adder.v")).to_vec(),
        )
        .unwrap();

        let buf = remove_first_line_from_buf(raw_buf);
        let ref_buf = include_bytes!(concat!(
            env!("CARGO_MANIFEST_DIR"),
            "/../circuit-gen-rs/tests/data/result_abc_full_adder.blif.blif"
        ));

        assert_eq!(buf, ref_buf, "failed {buf:#?} vs {ref_buf:#?}");
    }

    /// NOTE: `abc` writes a first line like "# Benchmark "full_add" written by ABC on Tue Jul 25 12:08:48 2023"
    /// cf https://github.com/berkeley-abc/abc/blame/d9f6af51afa7b84f8fb6d7c9d5cceef289b6f195/src/base/io/ioWriteBlif.c#L97
    /// so we must skip it if we want to compare with our reference file
    pub(crate) fn remove_first_line_from_buf(buf: Vec<u8>) -> Vec<u8> {
        use std::io::BufRead;
        let c = std::io::Cursor::new(buf);
        let mut lines = std::io::BufReader::new(c)
            .lines()
            .skip(1)
            .map(|x| x.unwrap())
            .collect::<Vec<String>>()
            .join("\n");

        // re-add the final `\n`
        lines.push('\n');

        lines.as_bytes().to_vec()
    }
}
