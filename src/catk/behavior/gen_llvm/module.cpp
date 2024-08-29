#include <catk/behavior/gen_llvm/driver.hpp>
#include <catk/behavior/gen_llvm/utils.hpp>
namespace catk::behavior::gen_llvm {


void Driver::handle_module(const catk::semantics::Module* smod) {
  curr_mod_ = std::make_unique<llvm::Module>("catk_mod", *get_llvm_context()); // TODO: need a module name
  auto* ft = get_llvm_type<void(void)>(*builder_);
  auto* func = create_function(llvm::cast<llvm::FunctionType>(ft), "catk_mod_construct");
  builder_->SetInsertPoint(&func->getEntryBlock());
  auto* sctx = smod->get_context();
  translate_context_def(sctx);
  translate_context_call("init", sctx, {}, {});
  builder_->CreateRet(nullptr);
}

std::unique_ptr<llvm::Module> Driver::translate_module(const catk::semantics::Module* smod) {
  handle_module(smod);
  return std::move(curr_mod_);
}

}