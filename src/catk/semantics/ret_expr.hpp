#pragma once
#include <catk/semantics/expr.hpp>

namespace catk::semantics {

class RetExpr : public Expr {
  Expr* rhs_ {nullptr};
public:
  Expr* rhs() const { return rhs_; };
  virtual std::vector<Expr*> dependencies() const;
  virtual void dump(catk::io::FmtStream& out) const;
  virtual Expr* clone() const;
  virtual Expr* deep_clone(SymbolTable& st) const;
  static RetExpr* from_ast(catk::syntax::AST& ast);
};


}