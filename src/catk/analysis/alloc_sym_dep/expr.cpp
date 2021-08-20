#pragma once
#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include <fmt/format.h>
#include "forward_decl.hpp"
namespace catk::analysis::alloc_sym_dep {


symdb::Symbol* expr(syntax::AST& ast, symdb::Symbol* up_dep, symdb::Symbol* context_sym) {
  assert(context_sym);
  // TODO: should use type switch will be better
  symdb::Symbol* res = nullptr;

  if(ast.is<syntax::Identifier>()) {
    auto name = ast.content();
    try {
      res = context_sym->accessable.at(name);
    } catch(const std::out_of_range& e) {
      throw std::runtime_error(fmt::format("unknown identifier: {}", name));
    }
  }
  auto& sym_db = catk::get_sym_db();
  res = sym_db.alloc();
  res->add_parent(context_sym);
  if(ast.is<syntax::IntLiteral>()) {
    res->set_literal(true);
    syntax::IntLiteral::interpret(ast, res->content);
  }
  if(ast.is<syntax::FPLiteral>()) {
    res->set_literal(true);
    syntax::FPLiteral::interpret(ast, res->content);
  }
  if(ast.is<syntax::LambdaLiteral>()) {
    CapturedSymbols captured_symbols;
    auto p_capture_list = syntax::LambdaLiteral::capture_list(ast);
    if(p_capture_list) {
      auto& capture_items = p_capture_list->children;
      for(auto&& item : capture_items) {
        auto& symtab = context_sym->accessable;
        auto item_name = item->content();
        auto captured_sym_iter = symtab.find(item_name);
        if( captured_sym_iter == symtab.end() ) {
          throw std::runtime_error(
            fmt::format("capture item: '{}' not found", item_name)
          );
        };
        auto& captured_sym = captured_sym_iter->second;
        captured_symbols[item_name] = captured_sym;
        item->set_symbol(*captured_sym);
      }
    }
    context(
      syntax::LambdaLiteral::stmts(ast), 
      context_sym, 
      captured_symbols
    );
  }
  // array, string literal currently not supported
  if(ast.is<syntax::StringLiteral>()) {
    throw std::runtime_error("string literal, not yet supported");
  }
  if(ast.is<syntax::ArrayLiteral>()) {
    throw std::runtime_error("array literal, not yet supported");
  }
  if(ast.is<syntax::Expr>()) {
    auto& func_ast      = syntax::Expr::function(ast);
    auto opnds_ast      = syntax::Expr::opnds(ast);
    auto opnds_labs_ast = syntax::Expr::opnd_labels(ast);
    res->ast = &func_ast;
    func_ast.set_symbol(*res);
    if(opnds_labs_ast.size() > 0) {
      assert(opnds_labs_ast.size() == opnds_ast.size());
      for(std::size_t i = 0; i < opnds_labs_ast.size(); i ++) {
        auto& param_expr = *opnds_ast.at(i);
        auto& param_name = *opnds_labs_ast.at(i);
        auto* param_expr_sym = expr(param_expr, res, context_sym);
        auto param_name_str = param_name.content();
        res->labeled_ref_to(param_name_str, param_expr_sym);
      }
    } else {
      for(auto&& ch : opnds_ast) {
        auto* ch_sym = expr(*ch, res, context_sym);
        res->ref_to(ch_sym);
      }
    }
  } else {
    // expression is a term
    res->ast = &ast; 
    ast.set_symbol(*res);
  }
  return res;
}

}