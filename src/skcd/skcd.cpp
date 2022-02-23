#include "skcd.h"

#include "skcd.pb.h"
#include "utils/files/utils_files.h"

namespace interstellar {

namespace skcd {

/**
 * Serialize to the target file using Protobug.
 */
void WriteToFile(boost::filesystem::path path, const BlifParser &blif_parser) {
  // TODO protobuf
  blif_parser.Getm();

  interstellarpbskcd::Skcd skcd_pb;

  skcd_pb.set_m(blif_parser.Getm());
  skcd_pb.set_n(blif_parser.Getn());
  skcd_pb.set_q(blif_parser.Getq());

  auto a = blif_parser.GetA();
  google::protobuf::RepeatedField<uint32_t> a_data(a.begin(), a.end());
  skcd_pb.mutable_a()->Swap(&a_data);

  auto b = blif_parser.GetB();
  google::protobuf::RepeatedField<uint32_t> b_data(b.begin(), b.end());
  skcd_pb.mutable_b()->Swap(&b_data);

  auto go = blif_parser.GetGO();
  google::protobuf::RepeatedField<uint32_t> go_data(go.begin(), go.end());
  skcd_pb.mutable_go()->Swap(&go_data);

  auto gt = blif_parser.GetGT();
  google::protobuf::RepeatedField<int32_t> gt_data;  // (gt.begin(), gt.end())
  gt_data.Reserve(gt.size());
  for (auto it : gt) {
    gt_data.AddAlreadyReserved(static_cast<int32_t>(it));
  }
  skcd_pb.mutable_gt()->Swap(&gt_data);

  auto o = blif_parser.GetO();
  google::protobuf::RepeatedField<uint32_t> o_data(o.begin(), o.end());
  skcd_pb.mutable_o()->Swap(&o_data);

  auto buf = skcd_pb.SerializeAsString();
  if (buf.empty()) {
    throw std::range_error("Serialization failed");
  }
  utils::WriteToFile(path, buf);
}

}  // namespace skcd

}  // namespace interstellar