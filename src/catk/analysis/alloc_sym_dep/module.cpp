#include "forward_decl.hpp"
#include <catk/utils.hpp>

namespace catk::analysis::alloc_sym_dep {

void module_(syntax::AST& ast, symdb::Symbol* parent) {
    auto& sym_db = catk::get_sym_db();
    auto& mod_sym = *sym_db.alloc();
    mod_sym.ast = &ast;
    mod_sym.add_parent(parent);
    mod_sym.set_module(true);

    ast.set_symbol(mod_sym);
    // resolve module as a context
    context(ast, &mod_sym, {}, {});
}

}