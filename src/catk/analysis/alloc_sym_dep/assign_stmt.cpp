#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include "forward_decl.hpp"
#include <fmt/format.h>
namespace catk::analysis::alloc_sym_dep {

void assign_stmt(const syntax::AST& ast, symdb::Symbol* parent) {
  auto& left_vals = syntax::AssignStmt::left_values(ast);
  auto& expr = syntax::AssignStmt::right_expr(ast);
  if(left_vals.size() > 1) {
    throw std::runtime_error("currently not support multiple value return");
  } 
  for(auto&& lv : left_vals) {
    auto id = lv->name();
    auto& sym_db = catk::get_sym_db();
    auto& sym = *sym_db.alloc();
    sym.ast = &ast; 
    sym.parent = parent; 
    sym.set_identifier(true);
    sym.set_locatable(true);
    sym.set_solid(false);
    sym.name = id;
    parent->accessable[id] = &sym;
    if(expr.is<syntax::LambdaLiteral>()) {
      CapturedSymbols captured_symbols;
      auto p_capture_list = syntax::LambdaLiteral::capture_list(expr);
      if(p_capture_list) {
        auto& capture_items = p_capture_list->children;
        for(auto&& item : capture_items) {
          auto& symtab = parent->accessable;
          auto item_name = item->name();
          auto captured_sym_iter = symtab.find(item_name);
          if( captured_sym_iter == symtab.end() ) {
            throw std::runtime_error(
              fmt::format("capture item: '{}' not found", item_name)
            );
          };
          captured_symbols[item_name] = captured_sym_iter->second;
        }
      }
      context(
        syntax::LambdaLiteral::stmts(expr), 
        parent, 
        captured_symbols
      );
    }
  }
}
}