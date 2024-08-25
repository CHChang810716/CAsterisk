#include <catk/behavior/gen_llvm/driver.hpp>
#include <avalon/mpl/virt_visit.hpp>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
namespace catk::behavior::gen_llvm {

using SemanticValues = avalon::mpl::TypeList<
  const catk::semantics::Constant,
  const catk::semantics::Symbol,
  const catk::semantics::FunctionalExpr,
  const catk::semantics::Context
>;

struct ValueTransForFuncExprVis {
  static constexpr std::uint8_t SI  = 0b00001;
  static constexpr std::uint8_t UI  = 0b00010;
  static constexpr std::uint8_t FP  = 0b00100;
  static constexpr std::uint8_t STR = 0b01000;
  static constexpr std::uint8_t NP  = 0b10000;
  static constexpr std::uint8_t ALLN = SI | UI | FP;

#define BOP_HANDLE(lhsflag, rhsflag, tbop, type_legalizer, handler) \
  if ( \
    (tbop == bop) && \
    (opnd0_flag & lhsflag) && \
    (opnd1_flag & rhsflag) \
  ) { \
    auto new_opnds = type_legalizer(opnds); \
    return handler(new_opnds[0], new_opnds[1]); \
  }
  // TODO: typeing, implict cast
  auto tl_match_ret_type(const llvm::SmallVector<llvm::Value*, 4>& opnds) const {
    auto new_opnds = opnds;
    for (unsigned i = 0; i < new_opnds.size(); ++i) {
      auto& v = new_opnds[i];
      if (v->getType() != res_type) {
        if (res_type->isFloatingPointTy()) {
          v = builder.CreateFPCast(v, res_type);
          continue;
        }
        if (s_res_type->is_signed_int()) {
          v = builder.CreateSExtOrTrunc(v, res_type);
          continue;
        }
        if (s_res_type->is_unsigned_int()) {
          v = builder.CreateZExtOrTrunc(v, res_type);
          continue;
        }
        rt_assert(false, "NYI - type legalization: " + s_opnds[i]->dump_str());
      }
    }
    return new_opnds;
  }
  
  auto tl_match_ret_bits(const llvm::SmallVector<llvm::Value*, 4>& opnds) const {
    auto new_opnds = opnds;
    for (unsigned i = 0; i < new_opnds.size(); ++i) {
      auto& v = new_opnds[i];
      if (v->getType()->getIntegerBitWidth() < res_type->getIntegerBitWidth()) {
        auto* stype = catk::get_type(s_opnds[i]);
        if (stype->is_signed_int()) {
          v = builder.CreateSExt(v, res_type);
          continue;
        }
        if (stype->is_unsigned_int()) {
          v = builder.CreateZExt(v, res_type);
          continue;
        }
        rt_assert(false, "NYI - type legalization: " + s_opnds[i]->dump_str());
      }
    }
    return new_opnds;
  }

  auto tl_check_same(const llvm::SmallVector<llvm::Value*, 4>& opnds) const {
    for (unsigned i = 1; i < s_opnds.size(); ++i) {
      rt_assert(catk::get_type(s_opnds[i]) == catk::get_type(s_opnds[i - 1]), 
        fmt::format("type not match: ({}), ({})", 
          s_opnds[i - 1]->dump_str(), s_opnds[i]->dump_str()
        )
      );
    }
    return opnds;
  }

  std::uint8_t resolve_flag(const llvm::Value* v, const catk::semantics::Expr* sv) const {
    auto* semty = catk::get_type(sv);
    if (!semty->is_primary()) return NP;
    if (semty->is_float()) return FP;
    if (semty->is_signed_int()) return SI;
    if (semty->is_unsigned_int()) return UI;
    rt_assert(false, "NYI type, expr: " + sv->dump_str());
    return 0;
  }
  inline llvm::Value* operator()(catk::semantics::BinOp bop) const {
    using namespace catk::semantics;
    auto opnds = translate_opnds();
    rt_assert(opnds.size() == 2, "binary operator only accept 2 operands");
    auto opnd0_flag = resolve_flag(opnds[0], s_opnds[0]);
    auto opnd1_flag = resolve_flag(opnds[1], s_opnds[1]);
    BOP_HANDLE(SI|UI, SI|UI, BOP_ADD, tl_match_ret_type, builder.CreateAdd);
    BOP_HANDLE(FP,    FP,    BOP_ADD, tl_match_ret_type, builder.CreateFAdd);
    BOP_HANDLE(SI|UI, SI|UI, BOP_SUB, tl_match_ret_type, builder.CreateSub);
    BOP_HANDLE(FP,    FP,    BOP_SUB, tl_match_ret_type, builder.CreateFSub);
    BOP_HANDLE(SI|UI, SI|UI, BOP_MUL, tl_match_ret_type, builder.CreateMul);
    BOP_HANDLE(FP,    FP,    BOP_MUL, tl_match_ret_type, builder.CreateFMul);
    BOP_HANDLE(SI,    SI,    BOP_DIV, tl_match_ret_type, builder.CreateSDiv);
    BOP_HANDLE(UI,    UI,    BOP_DIV, tl_match_ret_type, builder.CreateUDiv);
    BOP_HANDLE(FP,    FP,    BOP_DIV, tl_match_ret_type, builder.CreateFDiv);
    BOP_HANDLE(SI,    SI,    BOP_MOD, tl_match_ret_type, builder.CreateSRem);
    BOP_HANDLE(UI,    UI,    BOP_MOD, tl_match_ret_type, builder.CreateURem);
    BOP_HANDLE(FP,    FP,    BOP_MOD, tl_match_ret_type, builder.CreateFRem);
    BOP_HANDLE(SI|UI, SI|UI, BOP_AND, tl_match_ret_bits, builder.CreateAnd);
    BOP_HANDLE(SI|UI, SI|UI, BOP_OR,  tl_match_ret_bits, builder.CreateOr);
    BOP_HANDLE(SI|UI, SI|UI, BOP_XOR, tl_match_ret_bits, builder.CreateXor);
    BOP_HANDLE(SI,    SI,    BOP_LT,  tl_check_same,     builder.CreateICmpSLT);
    BOP_HANDLE(UI,    UI,    BOP_LT,  tl_check_same,     builder.CreateICmpULT);
    BOP_HANDLE(FP,    FP,    BOP_LT,  tl_check_same,     builder.CreateFCmpOLT);
    BOP_HANDLE(SI,    SI,    BOP_GT,  tl_check_same,     builder.CreateICmpSGT);
    BOP_HANDLE(UI,    UI,    BOP_GT,  tl_check_same,     builder.CreateICmpUGT);
    BOP_HANDLE(FP,    FP,    BOP_GT,  tl_check_same,     builder.CreateFCmpOGT);
    BOP_HANDLE(SI|UI, SI|UI, BOP_EQ,  tl_check_same,     builder.CreateICmpEQ);
    BOP_HANDLE(FP,    FP,    BOP_EQ,  tl_check_same,     builder.CreateFCmpOEQ);
    BOP_HANDLE(SI,    SI,    BOP_LE,  tl_check_same,     builder.CreateICmpSLE);
    BOP_HANDLE(UI,    UI,    BOP_LE,  tl_check_same,     builder.CreateICmpULE);
    BOP_HANDLE(FP,    FP,    BOP_LE,  tl_check_same,     builder.CreateFCmpOLE);
    BOP_HANDLE(SI,    SI,    BOP_GE,  tl_check_same,     builder.CreateICmpSGE);
    BOP_HANDLE(UI,    UI,    BOP_GE,  tl_check_same,     builder.CreateICmpUGE);
    BOP_HANDLE(FP,    FP,    BOP_GE,  tl_check_same,     builder.CreateFCmpOGE);
    BOP_HANDLE(SI|UI, SI|UI, BOP_NE,  tl_check_same,     builder.CreateICmpNE);
    BOP_HANDLE(FP,    FP,    BOP_NE,  tl_check_same,     builder.CreateFCmpONE);
    rt_assert(false, 
      fmt::format("no available binary operator for operands: ({}), ({})", 
        s_opnds[0]->dump_str(), s_opnds[1]->dump_str()
      )
    );
    return nullptr;
  }
  inline llvm::Value* operator()(catk::semantics::UnaryOp uop) const {
    using namespace catk::semantics;
    auto opnds = translate_opnds();
    rt_assert(opnds.size() == 1, "unary operator only accept 1 operands");
    auto& opnd = opnds[0];
    auto& s_opnd = s_opnds[0];
    switch (uop) {
    case UOP_ADD:
      return opnd;
    case UOP_INV:
      if (opnd->getType()->isIntegerTy())
        return builder.CreateNot(opnd);
      else
        rt_assert(false, "inverse operator translate failed: " + s_opnd->dump_str());
    case UOP_NOT:
      if (opnd->getType()->isIntegerTy())
        return builder.CreateICmpEQ(opnd, builder.getIntN(0, opnd->getType()->getIntegerBitWidth()));
      else
        rt_assert(false, "inverse operator translate failed: " + s_opnd->dump_str());
    case UOP_DEREF:
      return builder.CreateLoad(opnd->getType(), opnd);
    case UOP_ADDROF:
      return llvm::cast<llvm::LoadInst>(opnd)->getPointerOperand();
    default:
      rt_assert(false, "BUG: Uop code gen failed");
      return nullptr;
    }
  }
  inline llvm::Value* operator()(catk::semantics::IfElseOp top) const {
    rt_assert(s_opnds.size() == 3, "if else should have 3 operands");
    auto* PRes = builder.CreateAlloca(res_type);
    auto* curPt = &*builder.GetInsertPoint();
    llvm::Instruction* ThenPt;
    llvm::Instruction* ElsePt;
    auto* cond = driver.translate_value(s_opnds[0]);
    llvm::SplitBlockAndInsertIfThenElse(cond, curPt, &ThenPt, &ElsePt);
    builder.SetInsertPoint(ThenPt);
    auto* v0 = driver.translate_value(s_opnds[1]);
    builder.CreateStore(v0, PRes);
    builder.SetInsertPoint(ElsePt);
    auto* v1 = driver.translate_value(s_opnds[2]);
    builder.CreateStore(v1, PRes);
    builder.SetInsertPoint(curPt);
    return builder.CreateLoad(PRes->getType(), PRes);
  }
  inline llvm::Value* operator()(catk::semantics::Symbol* uf) const {
    auto opnds = translate_opnds();
    auto* callee_ctx = catk::get_type(uf)->get_lazy_context();
    return driver.translate_context_call(uf->get_name(), callee_ctx, s_opnds, opnds);
  }
  inline llvm::SmallVector<llvm::Value*, 4> translate_opnds() const {
    llvm::SmallVector<llvm::Value*, 4> opnds;
    for (auto& opnd : s_opnds) {
      opnds.push_back(driver.translate_value(opnd));
    }
    return opnds;
  }
  const catk::Type* s_res_type;
  llvm::Type* res_type;
  const std::vector<catk::semantics::Expr*>& s_opnds;
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
    auto* stype = catk::get_type(expr);
    ValueTransForFuncExprVis vis{
      stype, driver.translate_type(stype), expr->get_operands(), 
      driver, builder
    };
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
