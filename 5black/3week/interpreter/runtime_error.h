#pragma once

#include <stdexcept>

namespace Runtime {

struct RuntimeError : std::runtime_error {
  using std::runtime_error::runtime_error;
};

} /* namespace Runtime */