#include <catk/semantics.hpp>
#include <catk/behavior/unit.hpp>
#include <catk/behavior/gen_llvm.hpp>
#include <catk/behavior/memory.hpp>
namespace catk {

catk::behavior::Unit* get_unit_behavior(const catk::semantics::Module* mod) {
  // TODO: currently only support llvm, might support mixed code gen in future...
  auto llvm_unit = catk::behavior::gen_llvm_mod(mod);
  auto& unit = catk::behavior::DB::get().alloc<catk::behavior::Unit>(std::move(llvm_unit));
  return &unit;
}
  
} // namespace catk
