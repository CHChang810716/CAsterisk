#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include "forward_decl.hpp"
#include <fmt/format.h>
namespace catk::analysis::alloc_sym_dep {

void assign_stmt(syntax::AST& ast, symdb::Symbol* parent) {
  auto& left_vals = syntax::AssignStmt::left_values(ast);
  auto& right_expr = syntax::AssignStmt::right_expr(ast);
  if(left_vals.size() > 1) {
    throw std::runtime_error("currently not support multiple value return");
  } 
  for(auto&& lv : left_vals) {
    auto id = lv->content();
    auto& sym_db = catk::get_sym_db();
    auto& sym = *sym_db.alloc();
    sym.ast = &ast; 
    sym.add_parent(parent);
    sym.set_identifier(true);
    sym.set_locatable(true);
    sym.set_solid(false);
    sym.name = id;
    parent->accessable[id] = &sym;
    lv->set_symbol(sym);
    auto* expr_sym = expr(right_expr, &sym, parent);
  }
}
}