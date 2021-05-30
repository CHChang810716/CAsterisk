#include "forward_decl.hpp"
#include <catk/utils.hpp>

namespace catk::analysis {

void module_resolve(const syntax::AST& ast, symdb::Symbol* parent) {
    auto& sym_db = catk::get_sym_db();
    auto& mod_sym = *sym_db.alloc();
    mod_sym.ast = &ast;
    mod_sym.parent = parent;
    mod_sym.set_module(true);
    // resolve module as a context
    context_resolve(ast, &mod_sym);
}

}