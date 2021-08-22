#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include <fmt/format.h>
#include "forward_decl.hpp"
namespace catk::analysis::alloc_sym_dep {


static symdb::Symbol* identifier(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym) {
  auto name = ast.content();
  try {
    auto res = context_sym->accessable.at(name);
    res->ast = &ast; 
    ast.set_symbol(*res);
    res->set_expr(true);
    return res;
  } catch(const std::out_of_range& e) {
    throw std::runtime_error(fmt::format("unknown identifier: {}", name));
  }
}
static symdb::Symbol* int_literal(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym) {
  auto& sym_db = catk::get_sym_db();
  auto res = sym_db.alloc();
  res->add_parent(context_sym);
  res->set_literal(true);
  syntax::IntLiteral::interpret(ast, res->content);
  res->ast = &ast; 
  ast.set_symbol(*res);
  res->set_expr(true);
  return res;
}
static symdb::Symbol* fp_literal(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym) {
  auto& sym_db = catk::get_sym_db();
  auto res = sym_db.alloc();
  res->add_parent(context_sym);
  res->set_literal(true);
  syntax::FPLiteral::interpret(ast, res->content);
  res->ast = &ast; 
  ast.set_symbol(*res);
  res->set_expr(true);
  return res;
}
static symdb::Symbol* lambda_literal(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym) {
  auto& sym_db = catk::get_sym_db();
  auto res = sym_db.alloc();
  res->add_parent(context_sym);
  res->set_literal(true);
  CapturedSymbols captured_symbols;
  ParamSymbols param_symbols;
  auto p_capture_list = syntax::LambdaLiteral::capture_list(ast);
  if(p_capture_list) {
    auto& capture_items = p_capture_list->children;
    for(auto&& item : capture_items) {
      auto item_name = item->content();
      captured_symbols.push_back(item_name);
    }
  }
  // TODO: handler parameter
  auto param_asts = syntax::LambdaLiteral::params(ast);
  for(auto&& p : param_asts) {
    param_symbols.push_back(p->content());
  }
  context(
    syntax::LambdaLiteral::stmts(ast), 
    context_sym, 
    captured_symbols,
    param_symbols
  );
  res->ast = &ast; 
  ast.set_symbol(*res);
  res->set_expr(true);
  return res;
}
template<class ExprType>
static symdb::Symbol* comp_expr(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym) {
  auto& sym_db = catk::get_sym_db();
  auto res = sym_db.alloc();
  res->add_parent(context_sym);
  auto& func_ast      = ExprType::function(ast);
  auto opnds_ast      = ExprType::opnds(ast);
  res->ast = &func_ast;
  func_ast.set_symbol(*res);
  for(auto&& ch : opnds_ast) {
    auto* ch_sym = expr(*ch, res, context_sym);
    res->ref_to(ch_sym);
  }
  res->set_expr(true);
  return res;
}
static symdb::Symbol* fcall_expr(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym) {
  auto& sym_db = catk::get_sym_db();
  auto res = sym_db.alloc();
  res->add_parent(context_sym);
  auto& func_ast      = syntax::FCallExpr::function(ast);
  auto opnds_ast      = syntax::FCallExpr::opnds(ast);
  auto opnds_labs_ast = syntax::FCallExpr::opnd_labels(ast);
  res->ast = &func_ast;
  func_ast.set_symbol(*res);
  // For parameter binding syntex
  assert(opnds_labs_ast.size() == opnds_ast.size());
  for(std::size_t i = 0; i < opnds_labs_ast.size(); i ++) {
    auto& param_expr = *opnds_ast.at(i);
    auto& param_name = *opnds_labs_ast.at(i);
    auto* param_expr_sym = expr(param_expr, res, context_sym);
    auto param_name_str = param_name.content();
    res->labeled_ref_to(param_name_str, param_expr_sym);
  }
  res->set_expr(true);
  return res;
}
symdb::Symbol* expr(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym) {
  assert(context_sym);
  // TODO: should use type switch will be better

  if(ast.is<syntax::Identifier>()) {
    return identifier(ast, up_dep, context_sym);
  }
  if(ast.is<syntax::IntLiteral>()) {
    return int_literal(ast, up_dep, context_sym);
  }
  if(ast.is<syntax::FPLiteral>()) {
    return fp_literal(ast, up_dep, context_sym);
  }
  if(ast.is<syntax::LambdaLiteral>()) {
    return lambda_literal(ast, up_dep, context_sym);
  }
  // array, string literal currently not supported
  if(ast.is<syntax::StringLiteral>()) {
    throw std::runtime_error("string literal, not yet supported");
  }
  if(ast.is<syntax::ArrayLiteral>()) {
    throw std::runtime_error("array literal, not yet supported");
  }
  if(ast.is<syntax::RetContext>()) {
    throw std::runtime_error("return context as expression is not yet support");
  }
  if(ast.is<syntax::BinExpr>()) {
    return comp_expr<syntax::BinExpr>(ast, up_dep, context_sym);
  }
  if(ast.is<syntax::IfExpr>()) {
    return comp_expr<syntax::IfExpr>(ast, up_dep, context_sym);
  }
  if(ast.is<syntax::UnaryExpr>()) {
    return comp_expr<syntax::UnaryExpr>(ast, up_dep, context_sym);
  }
  if(ast.is<syntax::FCallExpr>()) {
    return fcall_expr(ast, up_dep, context_sym);
  }
}

}