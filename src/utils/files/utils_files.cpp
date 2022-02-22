#include "utils_files.h"

#include <absl/random/random.h>
#include <absl/strings/str_cat.h>
#include <glog/logging.h>

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

  DLOG(INFO) << "TempDir : created : " << _tmp_dir_path.generic_string();

  _cwd_before = boost::filesystem::current_path();
  boost::filesystem::current_path(_tmp_dir_path);
}

TempDir::~TempDir() {
  // restore the current dir
  boost::filesystem::current_path(_cwd_before);

  // delete the temp path
  boost::filesystem::remove_all(_tmp_dir_path);
}

boost::filesystem::path TempDir::GetPath() { return _tmp_dir_path; }

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

}  // namespace utils

}  // namespace interstellar