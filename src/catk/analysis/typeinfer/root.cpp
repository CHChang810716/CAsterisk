#include <catk/utils.hpp>
#include "forward_decl.hpp"

namespace catk::analysis::typeinfer {

void root(const syntax::AST& ast) {
  rt_assert(ast.is_root(), "AST not root.");
  rt_assert(ast.children.size() > 0, "root AST must have children");
  module_(ast, ast.symbol());
}

}