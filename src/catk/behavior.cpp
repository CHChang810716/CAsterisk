#pragma once
#include <catk/semantics.hpp>
#include <catk/behavior/unit.hpp>
#include <catk/behavior/gen_llvm.hpp>
#include <catk/behavior/memory.hpp>
namespace catk {

catk::behavior::Unit* make_unit(const catk::semantics::Module* mod) {
  // TODO: currently only support llvm, might support mixed code gen in future...
  auto* llvm_unit = catk::behavior::gen_llvm(mod);
  auto& unit = catk::behavior::DB::get().alloc<catk::behavior::Unit>(llvm_unit);
  return &unit;
}
  
} // namespace catk
