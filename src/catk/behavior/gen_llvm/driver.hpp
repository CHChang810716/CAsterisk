#pragma once
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <catk/semantics.hpp>
#include <catk/type.hpp>

#if defined(UNIT_TEST)
class CatkUnitTest;
#endif

namespace catk::behavior::gen_llvm {

class Driver {
  friend struct ValueTransVis;
  friend struct ValueTransForFuncExprVis;
  friend struct TypeTransVis;
#if defined(UNIT_TEST)
  friend ::CatkUnitTest;
#endif
public:
  Driver()
  : curr_mod_(nullptr)
  , builder_(std::make_unique<llvm::IRBuilder<>>(*get_llvm_context()))
  , curr_func_(nullptr)
  {}
  std::unique_ptr<llvm::Module> translate_module(const catk::semantics::Module* smod);
  llvm::Type* translate_type(const catk::Type* t);
  llvm::Value* translate_value(const catk::semantics::Expr* expr);
  llvm::Value* translate_context_def(const catk::semantics::Context* sctx);
  llvm::StructType* get_struct_type(llvm::StringRef name);
private:
  llvm::Function* create_function(llvm::FunctionType* ft, llvm::StringRef name);
  void handle_module(const catk::semantics::Module* smod);
  llvm::Value* translate_context_call(
    llvm::StringRef name, 
    const catk::semantics::Context* callee, 
    llvm::Value* callee_ctx_struct,
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
  std::unordered_map<const catk::semantics::Context*, llvm::Value*> slazy_ctx_struct_;
  std::unordered_map<const catk::semantics::Context*, llvm::Function*> typed_ctx_func_;
  std::unordered_map<const catk::semantics::Symbol*, llvm::Value*> symbol_storage_;
};

}