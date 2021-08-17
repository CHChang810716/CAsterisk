#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include <iostream>
#include "forward_decl.hpp"

namespace catk::analysis::typeinfer {

void context(
  syntax::AST& ast, 
  symdb::Symbol& sym
) {
  // loop over immutable function def
  for(auto&& ch_ast : ast.children) {
    auto& stmt_ast = *ch_ast;
    if(stmt_ast.is<syntax::AssignStmt>()) {
      assign_stmt(stmt_ast);
    }
  }
}

}