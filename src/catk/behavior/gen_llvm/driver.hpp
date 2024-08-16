#pragma once
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <catk/semantics.hpp>
#include <catk/type.hpp>
namespace catk::behavior::gen_llvm {

class Driver {
public:
  Driver()
  : curr_mod_(nullptr)
  , builder_(std::make_unique<llvm::IRBuilder<>>(*get_llvm_context()))
  , curr_func_(nullptr)
  {}
  llvm::Module* translate_module(const catk::semantics::Module* smod);
private:
  void handle_module(const catk::semantics::Module* smod) {
    curr_mod_ = std::make_unique<llvm::Module>("init_mod", *get_llvm_context()); // TODO: need a module name
  }
  void handle_context(const catk::semantics::Context* sctx) {
    rt_assert(sctx->is_immediate(), "context is not immediate");
    auto& ctx = *get_llvm_context();
    // TODO: create struct and function
    llvm::SmallVector<llvm::Value*, 4> caps;
    for (auto& scap : sctx->get_captures()) {
      // currently we only capture by reference
      llvm::Value* v = translate_capture_by_ref(scap);
      caps.push_back(v);
    }
    // make context struct type
    // llvm::StructType::create()

  }
  llvm::Value* translate_capture_by_ref(const catk::semantics::Expr* expr);
  inline static llvm::LLVMContext* get_llvm_context() {
     static thread_local llvm::LLVMContext context;
     return &context;
  }
  std::unique_ptr<llvm::Module> curr_mod_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  llvm::Function* curr_func_;
  std::unordered_map<catk::Type*, llvm::Type*> type_map_;
};

}