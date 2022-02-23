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