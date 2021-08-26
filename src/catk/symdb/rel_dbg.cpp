#include "rel_dbg.h"
#include <iostream>
#include <fmt/format.h>

namespace catk::symdb {

std::string graph_id(const Symbol& sym) {
  return fmt::format("x{}", (void*)std::addressof(sym));
}
void accessible(const Symbol& sym, std::ostream& out) {
  if(sym.accessable.size() == 0) return;
  out << graph_id(sym) << " -> {";
  bool is_first = true;
  for(auto&& [name, p_sym] : sym.accessable) {
    out << ((!is_first) ? "," : "") << graph_id(*p_sym) ;
    is_first = false;
  }
  out << "}" << "[color=\"green\"]" << std::endl;
}
void assign_to(const Symbol& sym, std::ostream& out) {
  if(!sym.is_identifier()) return;
  if(sym.get_ref_to().size() == 0) return;
  assert(sym.get_labed_ref_to().size() == 0);
  out << graph_id(sym) << " -> {";
  bool is_first = true;
  for(auto&& p_sym : sym.get_ref_to()) {
    out << ((!is_first) ? "," : "") << graph_id(*p_sym) ;
    is_first = false;
  }
  out << "}" << "[color=\"black\"]" << std::endl;
}
static void expr_tree_labeled_edge(const Symbol& sym, std::ostream& out) {
  assert(sym.get_ref_to().size() == sym.get_labed_ref_to().size());
  auto sym_id = graph_id(sym);
  for(std::size_t i = 0; i < sym.get_ref_to().size(); i ++) {
    auto* ref_sym = sym.get_ref_to().at(i);
    const auto& label = sym.get_labed_ref_to().at(i);
    auto ref_sym_id = graph_id(*ref_sym);
    out << sym_id << "->" << ref_sym_id 
      << fmt::format("[label=\"{}\", color=\"blue\"]", label) 
      << std::endl;
  }

}
void expr_tree(const Symbol& sym, std::ostream& out) {
  if(!sym.is_expr()) return;
  if(sym.get_ref_to().size() == 0) return;
  if(sym.get_labed_ref_to().size() > 0) {
    expr_tree_labeled_edge(sym, out);
    return;
  }
  out << graph_id(sym) << " -> {";
  bool is_first = true;
  for(auto&& p_sym : sym.get_ref_to()) {
    out << ((!is_first) ? "," : "") << graph_id(*p_sym) ;
    is_first = false;
  }
  out << "}" << "[color=\"blue\"]" << std::endl;
}
void module_node(const Symbol& sym, std::ostream& out) {
  if(!sym.is_module()) return;
  out << graph_id(sym) << " -> {";
  bool is_first = true;
  for(auto&& p_sym : sym.get_ref_to()) {
    out << ((!is_first) ? "," : "") << graph_id(*p_sym) ;
    is_first = false;
  }
  out << "}" << "[color=\"red\"]" << std::endl;

}
}