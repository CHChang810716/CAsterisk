#include <catk/type.hpp>
#include <catk/type/type.hpp>
#include <unordered_map>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/functional_expr.hpp>
#include <avalon/mpl/type_list.hpp>
#include <avalon/mpl/virt_visit.hpp>
#include <catk/type/memory.hpp>
#include <catk/type/context.hpp>
#include <catk/error/rule_violation.hpp>
#include <catk/semantics/constant.hpp>

namespace catk {

  using SCtx = catk::semantics::Context;
static thread_local std::unordered_map<const catk::semantics::Expr*, Type*> expr_type;
static thread_local std::unordered_map<const SCtx*, SCtx*> def_to_inst;

namespace detail {
using SemanticLeafTypes = avalon::mpl::TypeList<
  const catk::semantics::Constant,
  const catk::semantics::Symbol,
  const catk::semantics::RetExpr,
  const catk::semantics::FunctionalExpr,
  const catk::semantics::Context
>;

struct GetTypeFuncVisit {
  GetTypeFuncVisit(
    const std::vector<Type*>& opnd_tys
  )
  : opnd_tys_(opnd_tys)
  {}

  inline Type* operator()(catk::semantics::BinOp bop) const {
    // TODO: implement value range typing
    return opnd_tys_[0];
  }
  inline Type* operator()(catk::semantics::UnaryOp uop) const {
    auto& opnd = opnd_tys_[0];
    if (opnd == Type::get_undecided()) return opnd;
    switch(uop) {
    case catk::semantics::UOP_ADDROF:
      return type::Context::get().getType(opnd);
    case catk::semantics::UOP_DEREF:
      if (!opnd->is_pointer()) {
        throw catk::error::RuleViolation("only pointer can dereference");
      }
      return opnd->get_pointer_elem_ty();
    default:
      return opnd;
    }
  }
  inline Type* operator()(catk::semantics::IfElseOp top) const {
    if (opnd_tys_[1] != opnd_tys_[2]) {
      if (opnd_tys_[1] == Type::get_undecided()) return opnd_tys_[2];
      if (opnd_tys_[2] == Type::get_undecided()) return opnd_tys_[1];
      throw catk::error::RuleViolation("if-else clause type mismatch");
    }
    return opnd_tys_[1];
  }
  inline Type* operator()(catk::semantics::Symbol* uf) const {
    const catk::semantics::Context* lazy_ctx = nullptr;
    Type* lazy_ctx_type = getType(uf);
    lazy_ctx = lazy_ctx_type->get_lazy_context();
    rt_assert(lazy_ctx, "right hand side of user function symbol is not context: " + uf->dump_str());
    auto* imm_ctx = catk::getTypedContext(lazy_ctx, opnd_tys_);
    return getType(imm_ctx);
  }

private:
  const std::vector<Type*>& opnd_tys_;
};

struct GetTypeVisit {
  inline Type* operator()(const catk::semantics::Constant* expr) const {
    auto pri_ty = expr->get_type();
    return type::Context::get().getType(pri_ty);
  }
  inline Type* operator()(const catk::semantics::Symbol* expr) const {
    if (expr->rhs())
      return getType(expr->rhs());
    else
      return Type::get_undecided();
  }
  inline Type* operator()(const catk::semantics::RetExpr* expr) const {
    return getType(expr->rhs());
  }
  inline Type* operator()(const catk::semantics::FunctionalExpr* expr) const {
    std::vector<Type*> opnd_tys;
    for(auto& opnd : expr->get_operands()) {
      opnd_tys.push_back(getType(opnd));
    } 
    GetTypeFuncVisit fv(opnd_tys);
    return expr->visit_func(fv);
  }
  inline Type* operator()(const catk::semantics::Context* expr) const {
    if (expr->is_immediate())
      return getType(expr->get_return());
    return type::Context::get().getType(expr);
  }
  inline Type* operator()() const {
    rt_assert(false, "should not be here");
    return nullptr;
  }
};

}

const catk::semantics::Context* getTypedContext(const catk::semantics::Context* ctx, const std::vector<Type*>& opnd_tys) {
  rt_assert(!ctx->is_immediate(), "BUG: must not be immediate");
  auto& res = def_to_inst[ctx];
  if (!res) {
    auto* imm_ctx = static_cast<catk::semantics::Context*>(ctx->Expr::deep_clone({}));
    auto&& params = imm_ctx->params();
    rt_assert(params.size() == opnd_tys.size(), "BUG: type number and parameter number not matched");
    for (unsigned i = 0; i < params.size(); ++i) {
      auto& p = params[i];
      expr_type[p] = opnd_tys[i];
    }
    res = imm_ctx;
  }
  return res;
}

Type* getType(const catk::semantics::Expr* expr) {
  using namespace catk::semantics;
  auto& type = expr_type[expr];
  if (!type) {
    type = Type::get_undecided();
    type = avalon::mpl::virt_visit<
      catk::detail::SemanticLeafTypes, 
      catk::detail::GetTypeVisit
    >(expr);
  }
  return type;
}

}