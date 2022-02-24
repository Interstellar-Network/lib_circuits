#pragma once

#include <boost/filesystem.hpp>

namespace interstellar {

namespace CircuitPipeline {

void GenerateSkcd(boost::filesystem::path skcd_output_path, u_int32_t width,
                  u_int32_t height);

}  // namespace CircuitPipeline

}  // namespace interstellar