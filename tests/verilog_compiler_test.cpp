#include "verilog/verilog_compiler.h"

#include <absl/strings/str_cat.h>
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

  EXPECT_GT(boost::filesystem::file_size(output_path), 900);
  EXPECT_LT(boost::filesystem::file_size(output_path), 1000);
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

  EXPECT_GT(boost::filesystem::file_size(absl::StrCat(
                output_path.generic_string(), nb_threads - 1, ".blif")),
            900);
  EXPECT_LT(boost::filesystem::file_size(absl::StrCat(
                output_path.generic_string(), nb_threads - 1, ".blif")),
            1000);
}
