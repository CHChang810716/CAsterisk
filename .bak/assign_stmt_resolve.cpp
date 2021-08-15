#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include "forward_decl.hpp"
#include <fmt/format.h>
namespace catk::analysis::alloc_sym_dep {

template<class Left>
void throw_if_bad_left_numbers(const Left& left, std::size_t n, const std::string& label) {
  if(left.size() != n) {
    throw std::runtime_error(
      "syntax error: " + label + 
      " should have only " + std::to_string(n) + 
      " identifier"
    );
  } 
}
void string_def(const syntax::AST& left, const syntax::AST& right, symdb::Symbol* parent) {
  auto& sym_db = catk::get_sym_db();
  auto& type_db = catk::get_type_db();
  auto& str_type = type_db[symdb::CATK_STRING];
  auto& id = *sym_db.alloc(); 
  id.set_locatable(true);
  id.set_identifier(true);
  id.set_string(true);
  id.set_solid(true);
  id.name = left.content();
  id.type = &str_type;
  parent->accessable[id.name] = &id;

  auto& literal = *sym_db.alloc();
  literal.set_literal    (true);
  literal.set_string     (true);
  literal.set_solid      (true);
  literal.type = &str_type; 
  auto literal_with_quote = right.content();
  literal.content = literal_with_quote.substr(1, literal_with_quote.size() - 1);

  id.related.push_back(&literal);
  left.set_symbol(id);
  right.set_symbol(literal);
}

void lambda_assign_stmt(
  const syntax::AST& ast, 
  symdb::Symbol* parent
) {
  auto& left_vals = syntax::AssignStmt::left_values(ast);
  auto& expr = syntax::AssignStmt::right_expr(ast);
  if(expr.is<syntax::LambdaLiteral>()) {
    throw_if_bad_left_numbers(left_vals, 1, "lambda literal");
    auto& decl = left_vals[0];
    lambda_def(*decl, expr, parent);
  }
}

void value_assign_stmt(const syntax::AST& ast, symdb::Symbol* parent) {
  auto& left_vals = syntax::AssignStmt::left_values(ast);
  auto& expr = syntax::AssignStmt::right_expr(ast);
  if(expr.is<syntax::StringLiteral>()) {
    throw_if_bad_left_numbers(left_vals, 1, "string literal");
    auto& decl = left_vals[0];
    string_def(*decl, expr, parent);
  }
  if(expr.is<syntax::FPLiteral>()) {
    throw_if_bad_left_numbers(left_vals, 1, "float literal");
    auto& decl = left_vals[0];
    float_def(*decl, expr, parent);
  }
  if(expr.is<syntax::IntLiteral>()) {
    throw_if_bad_left_numbers(left_vals, 1, "integer literal");
    auto& decl = left_vals[0];
    int_def(*decl, expr, parent);
  }
  if(expr.is<syntax::Expr>()) {
  }
}
}