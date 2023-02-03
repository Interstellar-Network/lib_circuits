#pragma once

#include <cstdint>
#include <vector>

enum class GarblerInputsType : uint8_t {
  GARBLER_INPUTS_BUF,
  GARBLER_INPUTS_SEVEN_SEGMENTS,
  GARBLER_INPUTS_WATERMARK,
};

enum class EvaluatorInputsType : uint8_t {
  EVALUATOR_INPUTS_RND,
  EVALUATOR_INPUTS_CHOOSEN_BY_EVALUATOR,
  EVALUATOR_INPUTS_CHOOSEN_BY_GARBLER,
};

struct GarblerInputs {
  GarblerInputsType type;
  uint32_t length = 0;
};

struct EvaluatorInputs {
  EvaluatorInputsType type;
  uint32_t length = 0;
};

struct DisplayConfig {
  uint32_t width = 0;
  uint32_t height = 0;
  // cf drawable::DigitSegmentsType
  uint32_t segments_type = 0;
};

/**
 * IMPORTANT it MUST essentially match /protos/skcd/skcd.proto
 */
struct SkcdConfig {
  // INPUTS
  // Those are splitted into GARBLER:
  std::vector<GarblerInputs> garbler_inputs;
  // Then evaluator
  std::vector<EvaluatorInputs> evaluator_inputs;

  DisplayConfig display_config;
};