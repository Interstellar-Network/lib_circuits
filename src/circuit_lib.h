#pragma once

#include <boost/filesystem.hpp>

namespace interstellar {

namespace CircuitPipeline {

void GenerateSkcd(boost::filesystem::path skcd_output_path);

}  // namespace CircuitPipeline

}  // namespace interstellar