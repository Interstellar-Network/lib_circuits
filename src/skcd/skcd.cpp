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

#include "skcd.h"

#include <glog/logging.h>

#include "blif/gate_types.h"
#include "skcd.pb.h"

namespace {

void PrepareSkcdPb(const interstellar::BlifParser &blif_parser,
                   interstellarpbskcd::Skcd *skcd_pb) {
  // const auto &gates = ;
  for (const auto &gate : blif_parser.GetGates()) {
    auto skcd_gate = skcd_pb->add_gates();
    switch (gate.type) {
      case interstellar::SkcdGateType::ZERO:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::ZERO);
        break;

      case interstellar::SkcdGateType::NOR:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::NOR);
        break;

      case interstellar::SkcdGateType::AANB:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::AANB);
        break;

      case interstellar::SkcdGateType::INVB:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::INVB);
        break;

      case interstellar::SkcdGateType::NAAB:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::NAAB);
        break;

      case interstellar::SkcdGateType::INV:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::INV);
        break;

      case interstellar::SkcdGateType::XOR:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::XOR);
        break;

      case interstellar::SkcdGateType::NAND:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::NAND);
        break;

      case interstellar::SkcdGateType::AND:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::AND);
        break;

      case interstellar::SkcdGateType::XNOR:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::XNOR);
        break;

      case interstellar::SkcdGateType::BUF:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::BUF);
        break;

      case interstellar::SkcdGateType::AONB:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::AONB);
        break;

      case interstellar::SkcdGateType::BUFB:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::BUFB);
        break;

      case interstellar::SkcdGateType::NAOB:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::NAOB);
        break;

      case interstellar::SkcdGateType::OR:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::OR);
        break;

      case interstellar::SkcdGateType::ONE:
        skcd_gate->set_type(interstellarpbskcd::SkcdGateType::ONE);
        break;
    }
    skcd_gate->set_a(gate.a);
    skcd_gate->set_b(gate.b);
    skcd_gate->set_o(gate.o);
  }

  for (const auto &output : blif_parser.GetOutputs()) {
    (*skcd_pb->add_outputs()) = output;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Handle the "config"

  const auto &config = blif_parser.GetConfig();

  for (const auto &garbler_input : config.garbler_inputs) {
    auto input_data = (*skcd_pb->mutable_config()).add_garbler_inputs();
    switch (garbler_input.type) {
      case GarblerInputsType::GARBLER_INPUTS_BUF:
        input_data->set_type(
            interstellarpbskcd::GarblerInputsType::GARBLER_INPUTS_BUF);
        break;

      case GarblerInputsType::GARBLER_INPUTS_SEVEN_SEGMENTS:
        input_data->set_type(interstellarpbskcd::GarblerInputsType::
                                 GARBLER_INPUTS_SEVEN_SEGMENTS);
        break;

      case GarblerInputsType::GARBLER_INPUTS_WATERMARK:
        input_data->set_type(
            interstellarpbskcd::GarblerInputsType::GARBLER_INPUTS_WATERMARK);
        break;
    }
    input_data->set_length(garbler_input.length);
  }

  for (const auto &evaluator_input : config.evaluator_inputs) {
    auto input_data = (*skcd_pb->mutable_config()).add_evaluator_inputs();
    switch (evaluator_input.type) {
      case EvaluatorInputsType::EVALUATOR_INPUTS_RND:
        input_data->set_type(
            interstellarpbskcd::EvaluatorInputsType::EVALUATOR_INPUTS_RND);
        break;
    }
    input_data->set_length(evaluator_input.length);
  }

  auto config_display_config_data =
      (*skcd_pb->mutable_config()).mutable_display_config();
  config_display_config_data->set_height(config.display_config.height);
  config_display_config_data->set_width(config.display_config.width);
  config_display_config_data->set_segments_type(
      config.display_config.segments_type);
}

}  // anonymous namespace

namespace interstellar {

namespace skcd {

/**
 * Serialize to the target file using Protobuf.
 */
void WriteToFile(boost::filesystem::path path, const BlifParser &blif_parser) {
  interstellarpbskcd::Skcd skcd_pb;

  PrepareSkcdPb(blif_parser, &skcd_pb);

  std::fstream output(path.generic_string(),
                      std::ios::out | std::ios::trunc | std::ios::binary);
  auto ok = skcd_pb.SerializeToOstream(&output);
  if (!ok) {
    throw std::runtime_error("Serialization failed");
  }

  LOG(INFO) << "wrote skcd : " << path;
}

/**
 * Serialize directly to a buffer using Protobuf.
 */
std::string Serialize(const BlifParser &blif_parser) {
  interstellarpbskcd::Skcd skcd_pb;

  PrepareSkcdPb(blif_parser, &skcd_pb);

  std::string buf;
  skcd_pb.SerializeToString(&buf);

  return buf;
}

}  // namespace skcd

}  // namespace interstellar