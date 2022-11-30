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

#include "skcd.pb.h"

namespace {

void PrepareSkcdPb(const interstellar::BlifParser &blif_parser,
                   interstellarpbskcd::Skcd *skcd_pb) {
  const auto &a = blif_parser.GetA();
  google::protobuf::RepeatedField<uint32_t> a_data(a.begin(), a.end());
  skcd_pb->mutable_a()->Swap(&a_data);

  const auto &b = blif_parser.GetB();
  google::protobuf::RepeatedField<uint32_t> b_data(b.begin(), b.end());
  skcd_pb->mutable_b()->Swap(&b_data);

  const auto &go = blif_parser.GetGO();
  google::protobuf::RepeatedField<uint32_t> go_data(go.begin(), go.end());
  skcd_pb->mutable_go()->Swap(&go_data);

  const auto &gt = blif_parser.GetGT();
  google::protobuf::RepeatedField<int32_t> gt_data;  // (gt.begin(), gt.end())
  gt_data.Reserve(gt.size());
  for (auto it : gt) {
    gt_data.AddAlreadyReserved(static_cast<int32_t>(it));
  }
  skcd_pb->mutable_gt()->Swap(&gt_data);

  const auto &o = blif_parser.GetO();
  google::protobuf::RepeatedField<uint32_t> o_data(o.begin(), o.end());
  skcd_pb->mutable_o()->Swap(&o_data);

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