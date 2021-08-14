#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include <iostream>
#include "forward_decl.hpp"

namespace catk::analysis {

void context_resolve(const syntax::AST& ast, symdb::Symbol* parent) {
  auto& sym_db = catk::get_sym_db();
  auto& sym = *sym_db.alloc();
  sym.ast = &ast; 
  sym.set_context(true);
  sym.parent = parent; 
  // loop over immutable function def
  for(auto&& ch_ast : ast.children) {
    auto& stmt_ast = *ch_ast;
    if(stmt_ast.is<syntax::AssignStmt>()) {
      lambda_assign_stmt_resolve(stmt_ast, &sym);
    }
  }
  for(auto&& ch_ast : ast.children) {
    auto& stmt_ast = *ch_ast;
    if(stmt_ast.is<syntax::AssignStmt>()) {
      value_assign_stmt_resolve(stmt_ast, &sym);
    }
  }
}

}