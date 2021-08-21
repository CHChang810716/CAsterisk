#include "forward_decl.hpp"
#include <catk/utils.hpp>

namespace catk::analysis::typeinfer {

void module_(const syntax::AST& ast, symdb::Symbol& sym) {
    assert(sym.is_module());
    context(ast, ast.children.at(0)->symbol());
}

}