#include "forward_decl.hpp"
#include <catk/utils.hpp>
#include <ranges>
namespace catk::analysis {

std::vector<const syntax::AST*> module_resolve(const syntax::AST& ast, symdb::Symbol* parent) {
    auto& sym_db = catk::get_sym_db();
    auto& mod_sym = *sym_db.alloc();
    mod_sym.ast = &ast;
    mod_sym.parent = parent;
    mod_sym.set_module(true);
    ast.set_symbol(mod_sym);
    return {&ast};
}

}