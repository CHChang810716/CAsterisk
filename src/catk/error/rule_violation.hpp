#pragma once
#include <stdexcept>

namespace catk::error {

struct RuleViolation : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

} // namespace catk::error
