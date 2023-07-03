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

#include "utils_files.h"

#include <absl/random/random.h>
#include <absl/strings/str_cat.h>
#include <glog/logging.h>
#include <fstream>

// the final tmp dir is "/tmp/${kTmpSubDir}/${random_string}/"
static constexpr char kTmpSubDir[] = "interstellar-circuits";

namespace interstellar {

namespace utils {

std::string GetRandomString() {
  // TODO BitGen class member, and reuse it
  // TODO is this thread safe? Maybe use Boost or std::tmpnam?
  absl::BitGen bitgen;
  auto rand_uint64t = absl::Uniform<u_int64_t>(bitgen);  // From [0, 255]
  return absl::StrCat("", rand_uint64t);
}

TempDir::TempDir() {
  auto tmpname = GetRandomString();
  _tmp_dir_path =
      boost::filesystem::temp_directory_path() / kTmpSubDir / tmpname;

  // if the path already exists(improbable!); abort!
  // TODO better: SHOULD generate a new unique name and keep retrying
  if (boost::filesystem::exists(_tmp_dir_path)) {
    auto msg = absl::StrCat("TempDir : already exists : ",
                            _tmp_dir_path.generic_string());
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }

  // now create the directory(ies) if needed
  if (!boost::filesystem::create_directories(_tmp_dir_path)) {
    // Error: the path did not exist but could not be created
    auto msg = absl::StrCat("TempDir : could not create : ",
                            _tmp_dir_path.generic_string());
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }

  // DO NOT cwd into the temp dir; that WOULD break concurrency!
}

TempDir::~TempDir() {
  // delete the temp path
  boost::filesystem::remove_all(_tmp_dir_path);
}

boost::filesystem::path TempDir::GetPath() const { return _tmp_dir_path; }

void WriteToFile(boost::filesystem::path path, std::string_view content) {
  // TODO better error handling
  std::ofstream output_stream;
  output_stream.open(path.generic_string());
  output_stream << content;

  output_stream.close();
  // if anything went wrong(open or write); throw!
  if (!output_stream) {
    auto msg = absl::StrCat("WriteToFile : could not open/write : ",
                            path.generic_string());
    LOG(ERROR) << msg;
    throw std::runtime_error(msg);
  }

  DLOG(INFO) << "WriteToFile : written to : " << path.generic_string();
}

std::string ReadFile(boost::filesystem::path path) {
  // TODO avoid useless string copy x2?
  // TODO error handling
  std::ifstream input_stream_v(path, std::ios_base::binary);
  std::ostringstream sstr;
  sstr << input_stream_v.rdbuf();
  return sstr.str();
}

}  // namespace utils

}  // namespace interstellar