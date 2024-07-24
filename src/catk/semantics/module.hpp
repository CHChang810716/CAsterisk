#pragma once
#include <catk/syntax/ast.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/utils.hpp>

namespace catk::semantics {

class Module : public Expr {
  Context* context_;
  // TODO: module path
  
public:
  Module(Context* ctx) 
  : context_(ctx)
  {}

  static Module* from_ast(catk::syntax::AST& ast);
};

}