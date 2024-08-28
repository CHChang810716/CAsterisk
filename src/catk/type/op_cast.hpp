#pragma once
#include <catk/semantics/utils.hpp>
#include <catk/utils.hpp>

namespace catk::type {

struct BopCastInfo {
  catk::semantics::PrimaryType op0;
  catk::semantics::PrimaryType op1;
};

static inline BopCastInfo bop_cast(
  catk::semantics::PrimaryType op0,
  catk::semantics::PrimaryType op1
) {
  using namespace catk::semantics;
  // rt_assert(op0 != CATK_STRING && op1 != CATK_STRING, "no bop type cast for string");
  // if (op0 == CATK_FLOAT64 || op1 == CATK_FLOAT64) return {CATK_FLOAT64, CATK_FLOAT64};
  // if (op0 == CATK_FLOAT32 || op1 == CATK_FLOAT32) return {CATK_FLOAT32, CATK_FLOAT32};
  auto r = op0 > op1 ? op0 : op1;
  return {r, r};
}

} // namespace catk::type
