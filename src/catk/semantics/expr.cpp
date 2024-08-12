#include <cstdint>
#include <catk/syntax/ast.hpp>
#include <catk/syntax/utils.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/module.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/memory.hpp>
#include <catk/semantics/functional_expr.hpp>
#include <catk/semantics/symbol.hpp>
#include <catk/semantics/constant.hpp>
#include <unordered_map>
#include <string_view>
#include <variant>
#include <sstream>

namespace catk::semantics {

static Context* from_lambda_expr(catk::syntax::AST& ast) {
  std::vector<catk::syntax::AST*> params = catk::syntax::LambdaLiteral::params(ast);
  Context* ctx = Context::from_ast(catk::syntax::LambdaLiteral::body(ast), params, false);
  return ctx;
}

Expr* Expr::from_ast(catk::syntax::AST& ast) {
  if (ast.is_root()) {
    return Module::from_ast(ast);
  } else if (ast.is<catk::syntax::RetContext>()) {
    return Context::from_ast(ast, {});
  } else if (ast.is<catk::syntax::AssignStmt>()) {
    return Symbol::from_ast(ast);
  } else if (is_functional_expr(ast)) {
    return FunctionalExpr::from_ast(ast);
  } else if (ast.is<catk::syntax::Identifier>()) {
    return Symbol::from_ast(ast); 
  } else if (ast.is<catk::syntax::LambdaLiteral>()) {
    return from_lambda_expr(ast);
  } else if (ast.is<catk::syntax::IntLiteral>()) {
    return Constant::from_ast(ast);
  } else if (ast.is<catk::syntax::FPLiteral>()) {
    return Constant::from_ast(ast);
  } else if (ast.is<catk::syntax::StringLiteral>()) {
    rt_assert(false, "NYI: string");
  } else if (ast.is<catk::syntax::ArrayLiteral>()) {
    rt_assert(false, "NYI: array");
  } else {
    rt_assert(false, fmt::format("This AST has no semantics: <{}> {}", ast.name(), ast.content()));
  }
}

std::string Expr::dump_str() const {
  std::stringstream s;
  catk::io::FmtStream out(s);
  this->dump(out);
  out.flush();
  return s.str();
}

}