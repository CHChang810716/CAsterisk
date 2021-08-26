#include <catk/syntax.hpp>
#include <catk/symdb.hpp>
#include <iostream>
#include "forward_decl.hpp"

namespace catk::analysis::typeinfer {

symdb::Type* expr(symdb::Symbol& sym) {
  assert(sym.is_expr());
  auto* context = sym.get_parent();
  assert(context);
  auto& typedb = get_type_db();
  if(sym.is_literal()) {
    return typedb.find(sym.content);
  }
  // if(sym.is_param()) {
  //   return 
  // }
  if(sym.is_identifier()) { // is identifier (but not literal)
    return expr(*sym.get_ref_to().at(0));
  }

}

}