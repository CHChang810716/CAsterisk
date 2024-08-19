#include <catk/behavior/gen_llvm/driver.hpp>

namespace catk::behavior::gen_llvm {

llvm::Value* Driver::translate_context_def(const catk::semantics::Context* sctx) {
  if (sctx->is_immediate())
    return nullptr;
  llvm::Value*& res = slazy_ctx_struct_[sctx];
  if (res) return res;
  auto& ctx = *get_llvm_context();
  // make context struct
  llvm::SmallVector<llvm::Type*, 4> ctx_struct_mem_tys;
  auto& scaps = sctx->captures();
  for (auto& scap : scaps) {
    // TODO: currently we only capture by reference
    auto* scap_tar = dynamic_cast<catk::semantics::Symbol*>(scap->rhs());
    ctx_struct_mem_tys.push_back(symbol_storage_[scap_tar]->getType()->getPointerTo());
  }
  auto* ctx_struct_ty = llvm::StructType::create(ctx_struct_mem_tys);
  auto* ctx_struct = builder_->CreateAlloca(ctx_struct_ty);
  auto get_ctx_struct_mem = [&](unsigned i) {
    auto* rt = ctx_struct_ty->getStructElementType(i);
    return builder_->CreateGEP(rt, ctx_struct, {
      builder_->getInt8(0),
      builder_->getInt8(i)
    });
  };
  for (unsigned i = 0; i < scaps.size(); ++i) {
    auto& scap = scaps[i];
    auto* scap_tar = dynamic_cast<catk::semantics::Symbol*>(scap->rhs());
    llvm::Value* cap = symbol_storage_[scap_tar]; // pointer to value location
    builder_->CreateStore(cap, get_ctx_struct_mem(i));
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
  auto call_site = builder_->GetInsertPoint();
  llvm::Function* trans_curr_func = curr_func_;
  if (callee->is_immediate()) {
    sctx = callee;
  } else {
    rt_assert(!callee->is_immediate(), "callee context should be lazy");
    llvm::Value* ctx_struct = slazy_ctx_struct_[callee];
    rt_assert(ctx_struct, "context structure must not be null");
    // make function
    std::vector<catk::Type*> opnd_tys;
    for (auto& opnd : s_opnds) {
      opnd_tys.push_back(getType(opnd));
    }
    sctx = getTypedContext(callee, opnd_tys);
    auto* sret_ty = catk::getType(sctx);
    llvm::Type* rty = translate_type(sret_ty);
    llvm::SmallVector<llvm::Type*, 4> param_tys;
    param_tys.push_back(ctx_struct->getType()); // for captured context
    for (auto& scparam : sctx->params()) {
      auto* spty = catk::getType(scparam);
      param_tys.push_back(translate_type(spty));
    }
    auto* fty = llvm::FunctionType::get(rty, param_tys, false);
    auto* func = llvm::cast<llvm::Function>(curr_mod_->getOrInsertFunction(name, fty).getCallee());
    curr_func_ = func;
    call_site = builder_->GetInsertPoint();
    builder_->SetInsertPoint(&curr_func_->getEntryBlock());
    auto* ctx_struct_ty = ctx_struct->getType();
    auto get_ctx_struct_mem = [&](unsigned i) {
      auto* rt = ctx_struct_ty->getStructElementType(i);
      return builder_->CreateGEP(rt, ctx_struct, {
        builder_->getInt8(0),
        builder_->getInt8(i)
      });
    };
    auto& scaps = callee->captures();
    for (unsigned i = 0; i < scaps.size(); ++i) {
      auto& scap = scaps[i];
      symbol_storage_[scap] = get_ctx_struct_mem(i); // pointer to value location
    }
    auto& sparams = sctx->params();
    for (unsigned i = 0; i < sparams.size(); ++i) {
      auto& sparam = sparams[i];
      llvm::Value* sym_val = func->getOperand(i + 1);
      llvm::Value* sym_storage = builder_->CreateAlloca(sym_val->getType());
      builder_->CreateStore(sym_val, sym_storage);
      symbol_storage_[sparam] = sym_storage;
    }
  }
  llvm::Value* res = translate_value(sctx->get_return()->rhs());
  if (!callee->is_immediate()) {
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
    auto* llvm_callee = curr_func_;
    curr_func_ = trans_curr_func;
    builder_->SetInsertPoint(&*call_site);
    auto* ctx_struct = slazy_ctx_struct_[callee];
    llvm::SmallVector<llvm::Value*, 4> call_opnds;
    call_opnds.resize(opnds.size() + 1);
    call_opnds[0] = ctx_struct;
    for (unsigned i = 0; i < opnds.size(); ++i) {
      call_opnds[i + 1] = opnds[i];
    }
    res = builder_->CreateCall(llvm_callee, call_opnds);
  }
  return res;
}

}