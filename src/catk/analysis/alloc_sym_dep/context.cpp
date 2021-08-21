#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include <iostream>
#include "forward_decl.hpp"
#include <fmt/format.h>

namespace catk::analysis::alloc_sym_dep {

symdb::Symbol* context(
  syntax::AST& ast, // statement list
  symdb::Symbol* parent, 
  const CapturedSymbols& captured_symbols,
  const ParamSymbols& param_symbols
) {
  // we need a context "return" resolve
  auto& sym_db = catk::get_sym_db();
  auto& sym = *sym_db.alloc();
  sym.set_context(true);
  sym.add_parent(parent);
  for(auto&& name : captured_symbols) {
    assert(parent);
    auto& symtab = parent->accessable;
    auto captured_sym_iter = symtab.find(name);
    if( captured_sym_iter == symtab.end() ) {
      throw std::runtime_error(
        fmt::format("capture item: '{}' not found", name)
      );
    };
    sym.accessable[name] = captured_sym_iter->second;
  }
  for(auto&& name : param_symbols) {
    auto* param_sym = sym_db.alloc();
    param_sym->set_identifier(true);
    param_sym->set_locatable(true);
    param_sym->set_param(true);
    sym.accessable[name] = param_sym;
  }
  for(auto&& ch_ast : ast.children) {
    auto& stmt_ast = *ch_ast;
    if(stmt_ast.is<syntax::AssignStmt>()) {
      assign_stmt(stmt_ast, &sym);
    }
  }
  auto& ret_stmt = syntax::RetContext::ret_stmt(ast);
  auto& ret_expr = syntax::RetStmt::expr(ret_stmt);
  sym.ast = &ret_stmt;
  ret_stmt.set_symbol(sym);
  sym.ref_to(expr(ret_expr, &sym, &sym));
  return &sym;
}

}