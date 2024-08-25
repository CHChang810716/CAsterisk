#include <catk/behavior/gen_llvm/driver.hpp>

namespace catk::behavior::gen_llvm {

llvm::Function* Driver::create_function(llvm::FunctionType* ft, llvm::StringRef name) {
  auto* func = llvm::cast<llvm::Function>(curr_mod_->getOrInsertFunction(name, ft).getCallee());
  llvm::BasicBlock::Create(*get_llvm_context(), "catk_begin", func);
  curr_func_ = func;
  return func;
}
  
} // namespace catk::behavior::gen_lvm
