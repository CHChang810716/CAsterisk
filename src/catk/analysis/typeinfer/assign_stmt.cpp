#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include "forward_decl.hpp"
#include <fmt/format.h>
namespace catk::analysis::typeinfer {

void assign_stmt(syntax::AST& ast) {
  auto& left_vals = syntax::AssignStmt::left_values(ast);
  auto& expr = syntax::AssignStmt::right_expr(ast);
  if(left_vals.size() > 1) {
    throw std::runtime_error("currently not support multiple value return");
  }
  assert(left_vals.size() == 1);
  auto& lval = left_vals[0];
}

}