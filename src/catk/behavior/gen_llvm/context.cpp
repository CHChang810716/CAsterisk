#include <catk/behavior/gen_llvm/driver.hpp>

namespace catk::behavior::gen_llvm {

llvm::Value* Driver::translate_context_def(const catk::semantics::Context* sctx) {
  if (sctx->is_immediate())
    return nullptr;
  llvm::Value*& res = slazy_ctx_struct_[sctx];
  if (res) return res;
  auto& ctx = *get_llvm_context();
  // make context struct
  auto* ctx_struct_ty = translate_type(catk::get_type(sctx));
  auto* ctx_struct = builder_->CreateAlloca(ctx_struct_ty);
  auto get_ctx_struct_mem = [&](unsigned i) {
    auto* rt = ctx_struct_ty->getStructElementType(i);
    auto* gep = builder_->CreateGEP(ctx_struct_ty, ctx_struct, {
      builder_->getInt32(0),
      builder_->getInt32(i)
    });
    return builder_->CreateBitOrPointerCast(gep, rt->getPointerTo());
  };
  auto& scaps = sctx->captures();
  for (unsigned i = 0; i < scaps.size(); ++i) {
    auto& scap = scaps[i];
    auto* scap_tar = dynamic_cast<catk::semantics::Symbol*>(scap->rhs());
    llvm::Value* cap = translate_value(scap_tar);
    llvm::Value* ptr = get_ctx_struct_mem(i);
    builder_->CreateStore(cap, ptr);
  }
  slazy_ctx_struct_[sctx] = ctx_struct;
  struct_to_slazy_ctx_[ctx_struct] = sctx;
  return ctx_struct;
}

llvm::Value* Driver::translate_context_call(
  llvm::StringRef name, 
  const catk::semantics::Context* callee, 
  const std::vector<catk::semantics::Expr*>& s_opnds,
  const llvm::SmallVector<llvm::Value*, 4>& opnds
) {
  const catk::semantics::Context* sctx = nullptr;
  auto call_site = &*builder_->GetInsertPoint();
  llvm::Function* trans_curr_func = curr_func_;
  bool need_gen_func = false;
  if (callee->is_immediate()) {
    sctx = callee;
    llvm::Value* res = translate_value(sctx->get_return()->rhs());
    return res;
  } else {
    rt_assert(!callee->is_immediate(), "callee context should be lazy");
    llvm::Value* ctx_struct = slazy_ctx_struct_[callee];
    if (!ctx_struct) {
      ctx_struct = translate_context_def(callee);
    }
    // make function
    std::vector<catk::Type*> opnd_tys;
    for (auto& opnd : s_opnds) {
      opnd_tys.push_back(get_type(opnd));
    }
    sctx = get_typed_context(callee, opnd_tys);
    llvm::Function*& func = typed_ctx_func_[sctx];
    need_gen_func = (func == nullptr);
    if (need_gen_func) {
      auto* sret_ty = catk::get_type(sctx);
      llvm::Type* rty = translate_type(sret_ty);
      llvm::SmallVector<llvm::Type*, 4> param_tys;
      param_tys.push_back(ctx_struct->getType()); // for captured context
      for (auto& scparam : sctx->params()) {
        auto* spty = catk::get_type(scparam);
        param_tys.push_back(translate_type(spty));
      }
      auto* fty = llvm::FunctionType::get(rty, param_tys, false);
      func = create_function(fty, name);
      call_site = &*builder_->GetInsertPoint();
      curr_func_ = func;
      builder_->SetInsertPoint(&curr_func_->getEntryBlock());
      auto* ctx_struct_ty = ctx_struct->getType()->getPointerElementType();
      auto get_ctx_struct_mem = [&](unsigned i) {
        auto* rt = ctx_struct_ty->getStructElementType(i);
        auto* gep = builder_->CreateGEP(ctx_struct_ty, ctx_struct, {
          builder_->getInt32(0),
          builder_->getInt32(i)
        });
        return builder_->CreateLoad(rt, gep);
      };
      auto& scaps = callee->captures();
      for (unsigned i = 0; i < scaps.size(); ++i) {
        auto& scap = scaps[i];
        symbol_storage_[scap] = get_ctx_struct_mem(i); // pointer to value location
      }
      auto& sparams = sctx->params();
      for (unsigned i = 0; i < sparams.size(); ++i) {
        auto& sparam = sparams[i];
        llvm::Value* sym_val = func->getArg(i + 1);
        llvm::Value* sym_storage = builder_->CreateAlloca(sym_val->getType());
        builder_->CreateStore(sym_val, sym_storage);
        symbol_storage_[sparam] = sym_storage;
      }
      llvm::Value* res = translate_value(sctx->get_return()->rhs());
      builder_->CreateRet(res);
      for (auto& scap : sctx->captures()) {
        symbol_storage_.erase(scap);
      }
      for (auto& sparam : sctx->params()) {
        symbol_storage_.erase(sparam);
      }
      for (auto& local : sctx->locals()) {
        symbol_storage_.erase(local);
      }
      builder_->SetInsertPoint(call_site);
      curr_func_ = trans_curr_func;
    }
    llvm::SmallVector<llvm::Value*, 4> call_opnds;
    call_opnds.resize(opnds.size() + 1);
    call_opnds[0] = ctx_struct;
    for (unsigned i = 0; i < opnds.size(); ++i) {
      call_opnds[i + 1] = opnds[i];
    }
    return builder_->CreateCall(func, call_opnds);
  }
}

}