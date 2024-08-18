#include <catk/behavior/gen_llvm/driver.hpp>

namespace catk::behavior::gen_llvm {


void Driver::handle_module(catk::semantics::Module* smod) {
  curr_mod_ = std::make_unique<llvm::Module>("init_mod", *get_llvm_context()); // TODO: need a module name
  auto* sctx = smod->get_context();
  handle_context_def(sctx);
}

llvm::Module* Driver::translate_module(catk::semantics::Module* smod) {
  handle_module(smod);
  return curr_mod_.get();
}

}