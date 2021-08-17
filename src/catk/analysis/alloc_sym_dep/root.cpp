#include "forward_decl.hpp"
#include <catk/utils.hpp>
namespace catk::analysis::alloc_sym_dep {

void root(syntax::AST& ast) {
  rt_assert(ast.is_root(), "AST not root.");
  rt_assert(ast.children.size() > 0, "root AST must have children");
  module_(ast, nullptr);
}

}