#include "forward_decl.hpp"
#include <catk/utils.hpp>

namespace catk::analysis::typeinfer {

void module_(const syntax::AST& ast, symdb::Symbol& sym) {
    assert(sym.is_module());
    assert(sym.children.size() > 0);
    context(ast, *sym.children[0]);
}

}