#include <catk/behavior/gen_llvm/driver.hpp>

namespace catk::behavior::gen_llvm {

void Driver::handle_context_def( catk::semantics::Context* sctx) {
  rt_assert(!sctx->is_immediate(), "for context define, it must not be immediate");
  llvm::Value*& res = slazy_ctx_struct_[sctx];
  if (res) return;
  auto& ctx = *get_llvm_context();
  // make context struct
  llvm::SmallVector<llvm::Value*, 4> caps;
  llvm::SmallVector<llvm::Type*, 4> ctx_struct_mem_tys;
  for (auto& scap : sctx->get_captures()) {
    // TODO: currently we only capture by reference
    llvm::Value* v = translate_capture_by_ref(scap);
    caps.push_back(v);
    ctx_struct_mem_tys.push_back(v->getType()->getPointerTo());
  }
  auto* ctx_struct_ty = llvm::StructType::create(ctx_struct_mem_tys);
  auto* ctx_struct = builder_->CreateAlloca(ctx_struct_ty);
  auto get_ctx_struct_mem = [&](unsigned i) {
    return builder_->CreateGEP(ctx_struct, {
      builder_->getInt8(0),
      builder_->getInt8(i)
    });
  };
  for (unsigned i = 0; i < caps.size(); ++i) {
    auto& cap = caps[i];
    builder_->CreateStore(cap, get_ctx_struct_mem(i));
  }
  slazy_ctx_struct_[sctx] = ctx_struct;
  struct_to_slazy_ctx_[ctx_struct] = sctx;
}

void Driver::handle_context_call(
  llvm::StringRef name, 
  catk::semantics::Context* callee, 
  const std::vector<catk::semantics::Expr*>& opnds
) {
  rt_assert(!callee->is_immediate(), "callee context should be lazy");
  llvm::Value* ctx_struct = slazy_ctx_struct_[callee];
  rt_assert(ctx_struct, "context structure must not be null");
  // make function
  std::vector<catk::Type*> opnd_tys;
  for (auto& opnd : opnds) {
    opnd_tys.push_back(getType(opnd));
  }
  auto* sctx = getTypedContext(callee, opnd_tys);
  auto* sret_ty = catk::getType(sctx);
  llvm::Type* rty = translate_type(sret_ty);
  llvm::SmallVector<llvm::Type*, 4> param_tys;
  param_tys.push_back(ctx_struct->getType()); // for captured context
  for (auto& scparam : sctx->get_params()) {
    auto* spty = catk::getType(scparam);
    param_tys.push_back(translate_type(spty));
  }
  auto* fty = llvm::FunctionType::get(rty, param_tys, false);
  auto* func = llvm::cast<llvm::Function>(curr_mod_->getOrInsertFunction(name, fty));
  curr_func_ = func;
  builder_->SetInsertPoint(&curr_func_->getEntryBlock());
  handle_ret(sctx->get_return());
}
}