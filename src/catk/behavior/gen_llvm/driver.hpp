#pragma once
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <catk/semantics.hpp>
#include <catk/type.hpp>
namespace catk::behavior::gen_llvm {

struct ValueTransVis;
class Driver {
  friend ValueTransVis;
public:
  Driver()
  : curr_mod_(nullptr)
  , builder_(std::make_unique<llvm::IRBuilder<>>(*get_llvm_context()))
  , curr_func_(nullptr)
  {}
  llvm::Module* translate_module(catk::semantics::Module* smod);
  llvm::Type* translate_type(catk::Type* t);
  llvm::Value* translate_value(catk::semantics::Expr* expr);
private:
  llvm::Value* translate_context_def(catk::semantics::Context* sctx);
  void handle_module(catk::semantics::Module* smod);
  // void handle_context_def(catk::semantics::Context* sctx);
  void handle_context_call(
    llvm::StringRef name, 
    catk::semantics::Context* callee, 
    const std::vector<catk::semantics::Expr*>& opnds
  );
  void handle_ret(catk::semantics::RetExpr* sret);
  inline static llvm::LLVMContext* get_llvm_context() {
     static thread_local llvm::LLVMContext context;
     return &context;
  }
  std::unique_ptr<llvm::Module> curr_mod_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  llvm::Function* curr_func_;
  catk::semantics::Context* curr_sctx_;
  std::unordered_map<catk::Type*, llvm::Type*> type_map_;
  std::unordered_map<catk::semantics::Context*, llvm::Value*> slazy_ctx_struct_;
  std::unordered_map<llvm::Value*, catk::semantics::Context*> struct_to_slazy_ctx_;
  std::unordered_map<catk::semantics::Symbol*, llvm::Value*> symbol_storage_;
};

}