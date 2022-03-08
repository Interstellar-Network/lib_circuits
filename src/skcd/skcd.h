#pragma once

#include <boost/filesystem.hpp>

#include "blif/blif_parser.h"

namespace interstellar {

namespace skcd {

/**
 * Serialize to the target file using Protobuf.
 */
void WriteToFile(boost::filesystem::path path, const BlifParser &blif_parser);

/**
 * Serialize directly to a buffer using Protobuf.
 */
std::string Serialize(const BlifParser &blif_parser);

}  // namespace skcd

}  // namespace interstellar