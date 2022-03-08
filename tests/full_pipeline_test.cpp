#include <absl/strings/str_cat.h>
#include <gtest/gtest.h>

#include <filesystem>

#include "circuit_lib.h"
#include "resources.h"
#include "test_resources.h"
#include "utils/files/utils_files.h"

TEST(FullPipelineTest, BasicAdderOk) {
  auto tmp_dir = interstellar::utils::TempDir();
  auto verilog_input_path =
      absl::StrCat(interstellar::data_dir, "/verilog/adder.v");
  auto output_skcd_path = tmp_dir.GetPath() / "output.skcd";

  interstellar::CircuitPipeline::GenerateSkcd(output_skcd_path,
                                              {
                                                  verilog_input_path,
                                              });

  // TODO ideally we would want to compare functionally
  // ie are those the same gates? inputs? outputs? etc
  EXPECT_EQ(boost::filesystem::file_size(output_skcd_path), 80);
  auto output_str = interstellar::utils::ReadFile(output_skcd_path);
  auto expected_str = interstellar::utils::ReadFile(
      absl::StrCat(interstellar::test::test_data_dir, "/result_adder.skcd"));
  EXPECT_EQ(output_str, expected_str);
}