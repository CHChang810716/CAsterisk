#pragma once
#include <catk/syntax/ast.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/syntax/utils.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/op.hpp>
#include <catk/semantics/function.hpp>
#include <vector>

namespace catk::semantics {

using FunctionalExprASTs = avalon::mpl::TypeList<
  catk::syntax::BinExpr,
  catk::syntax::UnaryExpr,
  catk::syntax::FCallExpr,
  catk::syntax::IfExpr
>;


inline static bool is_functional_expr(catk::syntax::AST& ast) {
  return catk::syntax::is_in<FunctionalExprASTs>(ast);
}
class FunctionalExpr : public Expr {
  Function func_;
  std::vector<Expr*> opnds_;
public:
  void set_function(Function&& f) {
    func_ = std::move(f);
  }
  void set_opnds(std::vector<Expr*>&& opnds) {
    opnds_ = std::move(opnds);
  }
  template<class Visitor>
  auto visit_func(Visitor&& vis) const {
    return std::visit(
      std::forward<Visitor>(vis),
      func_.get_variant()
    );
  }
  bool is_user_function_call() const;
  Symbol* get_user_function() const;
  const auto& get_operands() const { return opnds_; }
  virtual void dump(catk::io::FmtStream& out) const;
  virtual std::vector<Expr*> dependencies() const;
  virtual Expr* clone() const;
  virtual Expr* deep_clone(SymbolTable& st) const;
  static FunctionalExpr* from_ast(catk::syntax::AST& ast);
};
using RHSExpr = FunctionalExpr;

}