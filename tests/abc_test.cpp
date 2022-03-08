#include <absl/strings/str_cat.h>
#include <absl/strings/str_split.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <thread>

#include "abc_wrapper/abc_wrapper.h"
#include "test_resources.h"
#include "utils/files/utils_files.h"

std::string StripCommentsFromBlif(std::string_view input) {
  std::string result;

  for (auto line : absl::StrSplit(input, "\n")) {
    if (!absl::StartsWith(line, "#")) {
      absl::StrAppend(&result, line);
    }
  }

  return result;
}

TEST(AbcWrapperTest, BasicAdderOk) {
  auto tmp_dir = interstellar::utils::TempDir();
  auto input_blif_path = absl::StrCat(interstellar::test::test_data_dir,
                                      "/result_verilog_full_adder.blif");
  auto output_blif_path = tmp_dir.GetPath() / "output.blif.blif";

  interstellar::ABC::Run(input_blif_path, output_blif_path.generic_string());

  EXPECT_GT(boost::filesystem::file_size(output_blif_path), 200);
  EXPECT_LT(boost::filesystem::file_size(output_blif_path), 300);
  // remove timestamped comments
  // "# Benchmark "full_add" written by ABC on Tue Mar  8 14:36:55 2022"
  // and all comments while we are at it b/c why not
  auto output_str =
      StripCommentsFromBlif(interstellar::utils::ReadFile(output_blif_path));
  auto expected_str = StripCommentsFromBlif(interstellar::utils::ReadFile(
      absl::StrCat(interstellar::test::test_data_dir,
                   "/result_abc_full_adder.blif.blif")));
  EXPECT_EQ(output_str, expected_str);
}

// Used to have an issue where starting multiple CompileVerilog in parallele
// would result in
// "ERROR: No such command: read_verilog (type 'help' for a command overview)"
TEST(AbcWrapperTest, ThreadSafeOk) {
  int nb_threads = 20;

  auto tmp_dir = interstellar::utils::TempDir();
  auto input_blif_path = absl::StrCat(interstellar::test::test_data_dir,
                                      "/result_verilog_full_adder.blif");
  auto output_path = tmp_dir.GetPath() / "output";

  std::vector<std::thread> threads(nb_threads);
  // spawn n threads:
  for (int i = 0; i < nb_threads; i++) {
    threads[i] = std::thread([&input_blif_path, &output_path, i] {
      interstellar::ABC::Run(
          input_blif_path,
          absl::StrCat(output_path.generic_string(), i, ".blif.blif"));
    });
  }

  for (auto& th : threads) {
    th.join();
  }

  EXPECT_GT(boost::filesystem::file_size(absl::StrCat(
                output_path.generic_string(), nb_threads - 1, ".blif.blif")),
            200);
  EXPECT_LT(boost::filesystem::file_size(absl::StrCat(
                output_path.generic_string(), nb_threads - 1, ".blif.blif")),
            300);
}
