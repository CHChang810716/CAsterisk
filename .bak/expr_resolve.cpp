#include "forward_decl.hpp"

namespace catk::analysis::alloc_sym_dep {

symdb::Symbol* expr(const syntax::AST& ast, symdb::Symbol* parent) {
  auto& sym_db = catk::get_sym_db();
  auto& expr_res = *sym_db.alloc();
  if(ast.is<syntax::BinExpr>()) {

  }
  return &expr_res;
}

}