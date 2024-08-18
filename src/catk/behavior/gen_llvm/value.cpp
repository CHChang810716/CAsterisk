#include <catk/behavior/gen_llvm/driver.hpp>
#include <avalon/mpl/virt_visit.hpp>
namespace catk::behavior::gen_llvm {

using SemanticValues = avalon::mpl::TypeList<
  catk::semantics::Constant,
  catk::semantics::Symbol,
  catk::semantics::FunctionalExpr,
  catk::semantics::Context
>;

struct ValueTransVis {
  inline llvm::Value* operator()(catk::semantics::Constant* expr) const {
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
  inline llvm::Value* operator()(catk::semantics::Symbol* expr) const {
    llvm::Value*& storage = driver.symbol_storage_[expr];
    if (!storage) {
      auto* sty = getType(expr);
      auto* ty = driver.translate_type(sty);
      storage = builder.CreateAlloca(ty);
    }
    return builder.CreateLoad(storage);
  }
  inline llvm::Value* operator()(catk::semantics::FunctionalExpr* expr) const {
  }
  inline llvm::Value* operator()(catk::semantics::Context* expr) const {
    return driver.translate_context_def(expr);
  }
  inline llvm::Value* operator()() const {
    rt_assert(false, "should not be here");
    return nullptr;
  }
  Driver& driver;
  llvm::IRBuilder<>& builder;
};

llvm::Value* Driver::translate_value(catk::semantics::Expr* expr) {
  ValueTransVis vis{*this, *this->builder_};
  return avalon::mpl::virt_visit<SemanticValues>(vis, expr);
}

}
