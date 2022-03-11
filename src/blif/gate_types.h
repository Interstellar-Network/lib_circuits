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

#pragma once

namespace interstellar {

/**
From lib_python/lib_python/gen_skcd/skcd.py

NOTE: there is already a class GateType in ABC
*/
enum class SkcdGateType : u_int8_t {
  ZERO,
  NOR,
  AANB,
  INVB,
  NAAB,
  INV,
  XOR,
  NAND,
  AND,
  XNOR,
  BUF,
  AONB,
  BUFB,
  NAOB,
  OR,
  ONE
};

}  // namespace interstellar