#include "forward_decl.hpp"
#include <catk/utils.hpp>
namespace catk::analysis {

void root_resolve(const syntax::AST& ast) {
  rt_assert(ast.is_root(), "AST not root.");
  rt_assert(ast.children.size() > 0, "root AST must have children");
  auto module_asts = module_resolve(
    ast, nullptr
  );
}

}