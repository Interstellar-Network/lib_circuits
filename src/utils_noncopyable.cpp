#include "utils_noncopyable.h"

#include <cstdio>
#include <exception>

namespace classutils {

noncopyable_terminate_on_copy::noncopyable_terminate_on_copy(
    const noncopyable_terminate_on_copy& /*unused*/) {
  printf("noncopyable_terminate_on_copy: called copy ctor: TERMINATING!\n");
  std::terminate();
}

}  // namespace classutils