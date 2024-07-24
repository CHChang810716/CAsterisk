#include <cstdint>
#include <catk/syntax/ast.hpp>
#include <catk/semantics/module.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/memory.hpp>

namespace catk::semantics {

// input module ast, output module main function

Module* Module::from_ast(catk::syntax::AST& ast) {
  rt_assert(ast.is_root(), "AST not root.");
  rt_assert(ast.children.size() > 0, "root AST must have children");
  return &DB::get().alloc<Module>(Context::from_ast(ast, {}));
}

}