#pragma once
#include <catk/syntax/ast.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/utils.hpp>

namespace catk::semantics {

class Module : public Expr {
  Context* context_ {nullptr};
  // TODO: module path
  
public:
  Module(Context* ctx) 
  : context_(ctx)
  {}
  Module() = default;
  virtual void dump(catk::io::FmtStream& out) const;
  virtual std::vector<Expr*> dependencies() const;
  virtual Expr* clone() const;
  virtual Expr* deep_clone(SymbolTable& st) const;
  const Context* get_context() const { return context_; }
  static Module* from_ast(catk::syntax::AST& ast);
};

}