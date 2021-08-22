#pragma once

#include "symbol.hpp"
#include <fmt/format.h>
namespace catk::symdb{

std::string graph_id(Symbol& sym);

void accessible(Symbol& sym, std::ostream& out);

void assign_to(Symbol& sym, std::ofstream& out);

template<class Malloc>
void sym_graph(
  const SymDB<Malloc>& symdb, 
  std::ostream& out
) {
  out << "digraph sym_graph {" << std::endl;
  for(auto&& sym : symdb) {
    std::stringstream label;
    auto label_item = "\\n{}";
    if(sym.is_context()) {
      label << fmt::format(label_item, "<context>");
    }
    if(sym.is_expr()) {
      label << fmt::format(label_item, "<expr>");
    }
    if(sym.is_function()) {
      label << fmt::format(label_item, "<function>");
    }
    if(sym.is_param()) {
      label << fmt::format(label_item, "<param>");
    }
    if(sym.is_identifier()) {
      label << fmt::format(label_item, 
        fmt::format("{}", sym.name())
      );
    }
    if(sym.is_literal()) {
      std::visit([&](auto&& v){
        label << fmt::format(label_item, "={}", v);
      }, sym.content);
    }
    auto node = fmt::format("{} [label=\"<{}>{}\"]", 
      graph_id(sym), 
      label.str(),
      sym.debug_id()
    );
    out << node << std::endl;
    accessible(sym, out);
  }
  out << "}" << std::endl;
}
    
} // namespace catk::symdb
