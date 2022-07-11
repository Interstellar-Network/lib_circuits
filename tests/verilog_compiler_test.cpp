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

#include "verilog/verilog_compiler.h"

#include <absl/strings/str_cat.h>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <thread>

#include "resources.h"
#include "utils/files/utils_files.h"

TEST(VerilogCompilerTest, BasicAdderOk) {
  auto tmp_dir = interstellar::utils::TempDir();
  auto output_path = tmp_dir.GetPath() / "output.blif";

  interstellar::VerilogHelper::CompileVerilog(
      {absl::StrCat(interstellar::data_dir, "/verilog/adder.v")},
      output_path.generic_string());

  // output size depends on compiler, etc
  // Expected: (boost::filesystem::file_size(output_path)) > (850), actual: 835
  // vs 850
  EXPECT_GT(boost::filesystem::file_size(output_path), 725);
  EXPECT_LT(boost::filesystem::file_size(output_path), 900);
}

// Used to have an issue where starting multiple CompileVerilog in parallele
// would result in
// "ERROR: No such command: read_verilog (type 'help' for a command overview)"
TEST(VerilogCompilerTest, ThreadSafeOk) {
  int nb_threads = 20;

  auto tmp_dir = interstellar::utils::TempDir();
  auto output_path = tmp_dir.GetPath() / "output";

  std::vector<std::thread> threads(nb_threads);
  // spawn n threads:
  for (int i = 0; i < nb_threads; i++) {
    threads[i] = std::thread([&output_path, i] {
      interstellar::VerilogHelper::CompileVerilog(
          {absl::StrCat(interstellar::data_dir, "/verilog/adder.v")},
          absl::StrCat(output_path.generic_string(), i, ".blif"));
    });
  }

  for (auto& th : threads) {
    th.join();
  }

  // output size depends on compiler, etc
  EXPECT_GT(boost::filesystem::file_size(absl::StrCat(
                output_path.generic_string(), nb_threads - 1, ".blif")),
            725);
  EXPECT_LT(boost::filesystem::file_size(absl::StrCat(
                output_path.generic_string(), nb_threads - 1, ".blif")),
            900);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  google::InitGoogleLogging(argv[0]);
  return RUN_ALL_TESTS();
}