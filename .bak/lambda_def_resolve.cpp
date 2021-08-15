#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include <atomic>
#include <fmt/format.h>
namespace catk::analysis::alloc_sym_dep {

void lambda_def(const syntax::AST& decl, const syntax::AST& expr, symdb::Symbol* parent) {
  static std::atomic_int g_lambda_id = 0;
  auto& sym_db = catk::get_sym_db();
  auto& type_db = catk::get_type_db();
  auto& func_type = *type_db.alloc();

  func_type.bits = 8;
  func_type.set_primary   (false);
  func_type.set_pointer   (false);
  func_type.set_mutable   (false);
  func_type.set_aggregate (true);
  func_type.set_complete  (true);

  auto lambda_id = g_lambda_id++;
  func_type.set_name      (fmt::format("lambda{}", lambda_id));

  auto& id = *sym_db.alloc();
  id.ast = &decl; 
  id.set_identifier(true);
  id.set_locatable(true);
  id.set_solid(true);
  id.name = decl.content();
  id.type = &func_type;
  parent->accessable[id.name] = &id;
  
  auto& literal = *sym_db.alloc();
  literal.ast = &expr;
  literal.set_function(true);
  literal.set_literal(true);
  literal.related.push_back(&id);
  literal.type = &func_type;

  id.related.push_back(&literal);
  decl.set_symbol(id);
  expr.set_symbol(literal);
  // function context is resolved by call
}

}