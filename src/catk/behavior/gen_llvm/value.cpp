#include <catk/behavior/gen_llvm/driver.hpp>
#include <avalon/mpl/virt_visit.hpp>
namespace catk::behavior::gen_llvm {

using SemanticValues = avalon::mpl::TypeList<
  const catk::semantics::Constant,
  const catk::semantics::Symbol,
  const catk::semantics::FunctionalExpr,
  const catk::semantics::Context
>;

struct ValueTransForFuncExprVis {
  inline llvm::Value* operator()(catk::semantics::BinOp bop) const {
    rt_assert(false, "NYI");
    return nullptr;
  }
  inline llvm::Value* operator()(catk::semantics::UnaryOp uop) const {
    rt_assert(false, "NYI");
    return nullptr;
  }
  inline llvm::Value* operator()(catk::semantics::IfElseOp top) const {
    rt_assert(false, "NYI");
    return nullptr;
  }
  inline llvm::Value* operator()(catk::semantics::Symbol* uf) const {
    auto* callee_ctx = catk::getType(uf)->get_lazy_context();
    return driver.translate_context_call(uf->get_name(), callee_ctx, s_opnds, opnds);
  }
  const std::vector<catk::semantics::Expr*>& s_opnds;
  const llvm::SmallVector<llvm::Value*, 4>& opnds;
  Driver& driver;
  llvm::IRBuilder<>& builder;
};
struct ValueTransVis {
  inline llvm::Value* operator()(const catk::semantics::Constant* expr) const {
    return std::visit(overloaded {
      [&](std::uint8_t v)       -> llvm::Constant* { return builder.getInt8(v); },
      [&](std::uint16_t v)      -> llvm::Constant* { return builder.getInt16(v); },
      [&](std::uint32_t v)      -> llvm::Constant* { return builder.getInt32(v); },
      [&](std::uint64_t v)      -> llvm::Constant* { return builder.getInt64(v); },
      [&](std::int8_t v)        -> llvm::Constant* { return builder.getInt8(v); },
      [&](std::int16_t v)       -> llvm::Constant* { return builder.getInt16(v); },
      [&](std::int32_t v)       -> llvm::Constant* { return builder.getInt32(v); },
      [&](std::int64_t v)       -> llvm::Constant* { return builder.getInt64(v); },
      [&](float v)              -> llvm::Constant* { return llvm::ConstantFP::get(builder.getContext(), llvm::APFloat(v)); },
      [&](double v)             -> llvm::Constant* { return llvm::ConstantFP::get(builder.getContext(), llvm::APFloat(v)); },
      [&](const std::string& s) -> llvm::Constant* { 
        rt_assert(false, "NYI string");
        return nullptr; 
      },
    }, expr->get_storage());
  }
  inline llvm::Value* operator()(const catk::semantics::Symbol* expr) const {
    llvm::Value*& storage = driver.symbol_storage_[expr];
    if (!storage) {
      // FIXME: if symbol is a context struct, need special handling.
      llvm::Value* rhs = driver.translate_value(expr->rhs());
      storage = builder.CreateAlloca(rhs->getType());
      builder.CreateStore(rhs, storage);
    }
    return builder.CreateLoad(storage->getType()->getPointerElementType(), storage);
  }
  inline llvm::Value* operator()(const catk::semantics::FunctionalExpr* expr) const {
    llvm::SmallVector<llvm::Value*, 4> opnds;
    for (auto& opnd : expr->get_operands()) {
      opnds.push_back(driver.translate_value(opnd));
    }
    ValueTransForFuncExprVis vis{expr->get_operands(), opnds, driver, builder};
    return expr->visit_func(vis);
  }
  inline llvm::Value* operator()(const catk::semantics::Context* expr) const {
    return driver.translate_context_def(expr);
  }
  inline llvm::Value* operator()() const {
    rt_assert(false, "should not be here");
    return nullptr;
  }
  Driver& driver;
  llvm::IRBuilder<>& builder;
};

llvm::Value* Driver::translate_value(const catk::semantics::Expr* expr) {
  ValueTransVis vis{*this, *this->builder_};
  return avalon::mpl::virt_visit<SemanticValues>(vis, expr);
}

}
