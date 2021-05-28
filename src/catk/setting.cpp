#include "setting.hpp"

#ifndef COMPILER_ARCH_BITS
# define COMPILER_ARCH_BITS 64
#endif
namespace catk {
std::uint8_t target_arch_bits = COMPILER_ARCH_BITS;

std::uint8_t target_runtime_pointer_bits() {
  return target_arch_bits;
}
}