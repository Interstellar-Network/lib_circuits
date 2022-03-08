#pragma once

#include <boost/filesystem.hpp>
#include <cstdio>
#include <string>
#include <string_view>

namespace interstellar {

namespace utils {

/**
 * Quick wrapper around boost::filesystem and std::filesystem to create and
 * manage a dir under /tmp that will deleted when not anymore(ie when it goes
 * out of scope)
 *
 */
class TempDir {
 public:
  TempDir();

  ~TempDir();

  boost::filesystem::path GetPath() const;

 private:
  boost::filesystem::path _tmp_dir_path;
};

void WriteToFile(boost::filesystem::path path, std::string_view content);

std::string ReadFile(boost::filesystem::path path);

}  // namespace utils

}  // namespace interstellar