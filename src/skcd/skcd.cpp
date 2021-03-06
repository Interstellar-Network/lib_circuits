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
  skcd_pb->set_m(blif_parser.Getm());
  skcd_pb->set_n(blif_parser.Getn());
  skcd_pb->set_q(blif_parser.Getq());

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

  const auto &circuit_data = blif_parser.GetCircuitData();
  skcd_pb->mutable_circuit_data()->set_gate_input_min(
      circuit_data.gate_input_min);
  skcd_pb->mutable_circuit_data()->set_gate_input_max(
      circuit_data.gate_input_max);
  skcd_pb->mutable_circuit_data()->set_gate_output_min(
      circuit_data.gate_output_min);
  skcd_pb->mutable_circuit_data()->set_gate_output_max(
      circuit_data.gate_output_max);
  skcd_pb->mutable_circuit_data()->mutable_layer_count()->Assign(
      circuit_data.layer_count.begin(), circuit_data.layer_count.end());
  skcd_pb->mutable_circuit_data()->mutable_input_gate_count()->Assign(
      circuit_data.input_gate_count.begin(),
      circuit_data.input_gate_count.end());

  for (auto const &[key, val] : blif_parser.GetConfig()) {
    (*skcd_pb->mutable_config())[key] = val;
  }
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