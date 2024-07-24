#include <cstdint>
#include <catk/syntax/ast.hpp>
#include <catk/syntax/utils.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/module.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/memory.hpp>
#include <catk/semantics/functional_expr.hpp>

namespace catk::semantics {

Context* from_lambda_expr(catk::syntax::AST& ast) {
  std::vector<catk::syntax::AST*> params = catk::syntax::LambdaLiteral::params(ast);
  Context* ctx = Context::from_ast(catk::syntax::LambdaLiteral::body(ast), params);
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
    rt_assert(false, ("This AST has no semantics: " + ast.content()));
  }
}

template<class FExpr>
struct FunctionalExprMaker {
  static FunctionalExpr* eval(catk::syntax::AST& ast) {
    FunctionalExpr& fexpr = DB::get().alloc<FunctionalExpr>();
    std::vector<Expr*> opnds;
    auto& next_lv = *(ast.children.at(0));
    auto ast_opnds = FExpr::opnds(next_lv);
    for (auto&& ast_opnd : ast_opnds) {
      opnds.push_back(Expr::from_ast(*ast_opnd));
    }
    fexpr.set_function(Function::from_expr<FExpr>(FExpr::function(next_lv)));
    fexpr.set_opnds(std::move(opnds));
    return &fexpr;
  }
};

FunctionalExpr* FunctionalExpr::from_ast(catk::syntax::AST& ast) {
  rt_assert(is_functional_expr(ast), "semantic parse error, not functional expr ast: " + ast.content());
  return catk::syntax::visit<FunctionalExprASTs, FunctionalExprMaker>(ast);
}

Expr* Symbol::rhs() const {
  return rhs_;
}

std::string Symbol::get_name_from_assign_stmt(catk::syntax::AST& ast) {
  rt_assert(ast.is<catk::syntax::AssignStmt>(), "must assign statement: " + ast.content());
  auto& lhs_syms_ast = catk::syntax::AssignStmt::left_list(ast);
  auto& lhs_syms = lhs_syms_ast.children;
  rt_assert(lhs_syms.size() == 1, "NYI: multiple LHS: " + lhs_syms_ast.content());
  auto& lhs_sym = *lhs_syms.at(0);
  std::string sym_name = lhs_sym.string();
  return sym_name;
}

Symbol* Symbol::from_ast(catk::syntax::AST& ast) {
  // search symbol from current context
  if (ast.is<catk::syntax::Identifier>()) {
    auto* ctx = get_current_context();
    if (ctx) {
      auto *res = ctx->get_symbol(ast.string());
      if (res) return res;
    }
    rt_assert(false, "identifier not defined: " + ast.string());
  }
  Symbol& sym = DB::get().alloc<Symbol>();
  if (ast.is<catk::syntax::Param>()) {
    sym.name_ = catk::syntax::Param::left_id(ast).string();
  } else {
    sym.name_ = get_name_from_assign_stmt(ast);
    auto& rhs_ast = catk::syntax::AssignStmt::right_expr(ast);
    sym.rhs_ = Expr::from_ast(rhs_ast);
  }
  return &sym;
}

RetExpr* RetExpr::from_ast(catk::syntax::AST& ast) {
  rt_assert(ast.is<catk::syntax::RetStmt>(), "must return statement: " + ast.content());
  RetExpr& ret = DB::get().alloc<RetExpr>();
  catk::syntax::AST& r_expr = catk::syntax::RetStmt::expr(ast);
  ret.rhs_ = Expr::from_ast(r_expr);
  return &ret;
}

using ConstantASTs = avalon::mpl::TypeList<
  catk::syntax::IntLiteral,
  catk::syntax::FPLiteral
  // catk::syntax::StringLiteral
>;

template<class SyntaxExpr>
struct ConstantMaker {
  static Constant* eval(catk::syntax::AST& ast) {
    Constant& cv = DB::get().alloc<Constant>();
    SyntaxExpr::interpret(ast, cv.get_storage());
    return &cv;
  }
};

Constant* Constant::from_ast(catk::syntax::AST& ast) {
  rt_assert(catk::syntax::is_in<ConstantASTs>(ast), "must constant value: " + ast.content());
  return catk::syntax::visit<ConstantASTs, ConstantMaker>(ast);
}

}