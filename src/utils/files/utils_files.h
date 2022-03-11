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