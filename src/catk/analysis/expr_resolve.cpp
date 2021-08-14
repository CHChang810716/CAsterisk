#include "forward_decl.hpp"

namespace catk::analysis {

symdb::Symbol* expr_resolve(const syntax::AST& ast, symdb::Symbol* parent) {
  auto& sym_db = catk::get_sym_db();
  auto& expr_res = *sym_db.alloc();
  if(ast.is<syntax::BinExpr>()) {

  }
  return &expr_res;
}

}