#include <catk/semantics/ret_expr.hpp>
#include <catk/utils.hpp>
#include <catk/syntax/ast.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/semantics/memory.hpp>

namespace catk::semantics {

RetExpr* RetExpr::from_ast(catk::syntax::AST& ast) {
  rt_assert(ast.is<catk::syntax::RetStmt>(), "must return statement: " + ast.content());
  RetExpr& ret = DB::get().alloc<RetExpr>();
  catk::syntax::AST& r_expr = catk::syntax::RetStmt::expr(ast);
  ret.rhs_ = Expr::from_ast(r_expr);
  return &ret;
}

void RetExpr::dump(catk::io::FmtStream& out) const {
  out << "ret ";
  rhs_->dump(out);
};

std::vector<Expr*> RetExpr::dependencies() const {
  if (!rhs_) return {};
  return {rhs_};
}

Expr* RetExpr::clone() const {
  return clone_impl<RetExpr>();
}

Expr* RetExpr::deep_clone(SymbolTable& st) const {
  RetExpr* re = &DB::get().alloc<RetExpr>();
  re->rhs_ = rhs_->deep_clone(st);
  return re;
}

}