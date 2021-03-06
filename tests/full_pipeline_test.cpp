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

#include <absl/strings/str_cat.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <thread>

#include "circuit_lib.h"
#include "resources.h"
#include "test_resources.h"
#include "utils/files/utils_files.h"

using namespace interstellar;

TEST(FullPipelineTest, BasicAdderToFileOk) {
  auto tmp_dir = utils::TempDir();
  auto verilog_input_path = absl::StrCat(data_dir, "/verilog/adder.v");
  auto output_skcd_path = tmp_dir.GetPath() / "output.skcd";

  circuits::GenerateSkcd(output_skcd_path, {
                                               verilog_input_path,
                                           });

  // TODO ideally we would want to compare functionally
  // ie are those the same gates? inputs? outputs? etc
  EXPECT_EQ(boost::filesystem::file_size(output_skcd_path), 80);
  auto output_str = utils::ReadFile(output_skcd_path);
  auto expected_str =
      utils::ReadFile(absl::StrCat(test::test_data_dir, "/result_adder.skcd"));
  EXPECT_EQ(output_str, expected_str);
}

// Version that return a buffer instead of writing a file
// IMPORTANT: this is the overload used by api_circuits
TEST(FullPipelineTest, BasicAdderToBufferOk) {
  auto tmp_dir = utils::TempDir();
  auto verilog_input_path = absl::StrCat(data_dir, "/verilog/adder.v");

  std::string buf = circuits::GenerateSkcd({
      verilog_input_path,
  });

  // TODO ideally we would want to compare functionally
  // ie are those the same gates? inputs? outputs? etc
  EXPECT_EQ(buf.size(), 80);
  auto expected_str =
      utils::ReadFile(absl::StrCat(test::test_data_dir, "/result_adder.skcd"));
  EXPECT_EQ(buf, expected_str);
}

TEST(FullPipelineTest, ThreadSafeOk) {
  int nb_threads = 20;

  auto tmp_dir = utils::TempDir();
  auto verilog_input_path = absl::StrCat(data_dir, "/verilog/adder.v");
  auto output_path = tmp_dir.GetPath() / "output";

  std::vector<std::thread> threads(nb_threads);
  // spawn n threads:
  for (int i = 0; i < nb_threads; i++) {
    threads[i] = std::thread([&verilog_input_path, &output_path, i] {
      circuits::GenerateSkcd(
          absl::StrCat(output_path.generic_string(), i, ".skcd"),
          {
              verilog_input_path,
          });
    });
  }

  for (auto& th : threads) {
    th.join();
  }

  EXPECT_EQ(boost::filesystem::file_size(absl::StrCat(
                output_path.generic_string(), nb_threads - 1, ".skcd")),
            80);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}