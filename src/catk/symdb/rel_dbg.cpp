#include "rel_dbg.h"
#include <iostream>
#include <fmt/format.h>

namespace catk::symdb {

std::string graph_id(const Symbol& sym) {
  return fmt::format("x{}", std::addressof(sym));
}
void accessible(const Symbol& sym, std::ostream& out) {
  out << graph_id(sym) << "{";
  bool is_first = true;
  for(auto&& [name, p_sym] : sym.accessable) {
    out << graph_id(*p_sym) << 
      !is_first ? "," : ""
    ;
    is_first = false;
  }
  out << "}" << "[color=\"green\"]" << std::endl;
}
void assign_to(const Symbol& sym, std::ostream& out) {
  if(!sym.is_identifier()) return;
  out << graph_id(sym) << "{";
  bool is_first = true;
  // for(auto&& [name, p_sym] : sym.get_ref_to()) {
  //   out << graph_id(*p_sym) << 
  //     !is_first ? "," : ""
  //   ;
  //   is_first = false;
  // }
  out << "}" << "[color=\"green\"]" << std::endl;
}

}