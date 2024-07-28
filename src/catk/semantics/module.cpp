#include <cstdint>
#include <catk/syntax/ast.hpp>
#include <catk/semantics/module.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/memory.hpp>
#include <catk/io/fmt_stream.hpp>

namespace catk::semantics {

// input module ast, output module main function

Module* Module::from_ast(catk::syntax::AST& ast) {
  rt_assert(ast.is_root(), "A module ast should be root ast");
  // rt_assert(ast.children.size() == 1, "Module should include one context children.");
  return &DB::get().alloc<Module>(Context::from_ast(ast, {}));
}

void Module::dump(catk::io::FmtStream& out) const {
  context_->dump(out);
}

std::vector<Expr*> Module::dependencies() const {
  return {};
}
}