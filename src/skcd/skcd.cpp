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

  utils::WriteToFile(path, "TODO");
}

}  // namespace skcd

}  // namespace interstellar