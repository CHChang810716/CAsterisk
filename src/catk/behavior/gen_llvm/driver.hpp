#pragma once
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <catk/semantics.hpp>
#include <catk/type.hpp>
namespace catk::behavior::gen_llvm {

class Driver {
  friend struct ValueTransVis;
  friend struct ValueTransForFuncExprVis;
public:
  Driver()
  : curr_mod_(nullptr)
  , builder_(std::make_unique<llvm::IRBuilder<>>(*get_llvm_context()))
  , curr_func_(nullptr)
  {}
  llvm::Module* translate_module(const catk::semantics::Module* smod);
  llvm::Type* translate_type(const catk::Type* t);
  llvm::Value* translate_value(const catk::semantics::Expr* expr);
  llvm::Value* translate_context_def(const catk::semantics::Context* sctx);
private:
  void handle_module(const catk::semantics::Module* smod);
  llvm::Value* translate_context_call(
    llvm::StringRef name, 
    const catk::semantics::Context* callee, 
    const std::vector<catk::semantics::Expr*>& s_opnds,
    const llvm::SmallVector<llvm::Value*, 4>& opnds
  );
  inline static llvm::LLVMContext* get_llvm_context() {
     static thread_local llvm::LLVMContext context;
     return &context;
  }
  std::unique_ptr<llvm::Module> curr_mod_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  llvm::Function* curr_func_;
  std::unordered_map<const catk::Type*, const llvm::Type*> type_map_;
  std::unordered_map<const catk::semantics::Context*, llvm::Value*> slazy_ctx_struct_;
  std::unordered_map<llvm::Value*, const catk::semantics::Context*> struct_to_slazy_ctx_;
  std::unordered_map<const catk::semantics::Symbol*, llvm::Value*> symbol_storage_;
};

}