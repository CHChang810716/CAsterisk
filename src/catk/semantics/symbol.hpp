#pragma once
#include <catk/semantics/expr.hpp>

namespace catk::semantics {

class Symbol : public Expr {
  std::string name_ {""};
  Expr* rhs_ {nullptr};
public:
  virtual const char* get_name() const {
    return name_.c_str();
  }
  Expr* rhs() const;
  virtual bool has_name() const { return true; }
  virtual std::vector<Expr*> dependencies() const;
  virtual void dump(catk::io::FmtStream& out) const;
  virtual Expr* clone() const;
  virtual Expr* deep_clone(SymbolTable& st) const;
  static Symbol* from_ast(catk::syntax::AST& ast);
  static std::string get_name_from_assign_stmt(catk::syntax::AST& ast);
};

}