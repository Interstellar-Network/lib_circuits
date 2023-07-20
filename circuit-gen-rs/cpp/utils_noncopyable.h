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

namespace classutils {

class noncopyable {
 protected:
  // NOLINTNEXTLINE(modernize-use-equals-default)
  noncopyable() {}
  // NOLINTNEXTLINE(modernize-use-equals-default)
  ~noncopyable() {}

  noncopyable(noncopyable const &) = delete;
  void operator=(noncopyable const &x) = delete;
};

/**
 * Special version, because vector<SomeClass>.reserve() requires the copy ctor
 * to be defined.
 * It SHOULD NOT be called at runtime, unless the vector was not correctly sized
 * the first time, in which case it would copy the elements around.
 * Therefore: call std::terminate() if the copy ctor is called!
 */
class noncopyable_terminate_on_copy {
 protected:
  // NOLINTNEXTLINE(modernize-use-equals-default)
  noncopyable_terminate_on_copy() {}
  // NOLINTNEXTLINE(modernize-use-equals-default)
  ~noncopyable_terminate_on_copy() {}

  noncopyable_terminate_on_copy(const noncopyable_terminate_on_copy &);
  void operator=(noncopyable_terminate_on_copy const &x) = delete;
};

}  // namespace classutils