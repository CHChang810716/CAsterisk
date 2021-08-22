#pragma once

#include "symbol.hpp"
#include <fmt/format.h>
namespace catk::symdb{

std::string graph_id(const Symbol& sym);
void accessible(const Symbol& sym, std::ostream& out);
void assign_to(const Symbol& sym, std::ostream& out);
void expr_tree(const Symbol& sym, std::ostream& out);


template<class DB>
void sym_graph(
  const DB& symdb, 
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
    // if(sym.is_function()) {
    //   label << fmt::format(label_item, "<function>");
    // }
    if(sym.is_param()) {
      label << fmt::format(label_item, "<param>");
    }
    if(sym.is_identifier()) {
      label << fmt::format(label_item, 
        fmt::format("{}", sym.name)
      );
    }
    // if(sym.is_literal()) {
    //   if(sym.is_function()) {
    //     label << fmt::format(label_item, "={}", "<lambda>");
    //   } else {
    //     std::visit([&](auto&& v){
    //       label << fmt::format(label_item, "={}", v);
    //     }, sym.content);
    //   }
    // }
    if(sym.is_expr()) {
      if(sym.is_function()) {
        label << fmt::format(label_item, "<lambda>");
      } else {
        label << fmt::format(label_item, sym.ast->content());
      }
      // label << fmt::format(label_item, sym.ast->content());
    }
    auto node = fmt::format("{} [label=\"<{}>{}\"]", 
      graph_id(sym), 
      sym.debug_id(),
      label.str()
    );
    out << node << std::endl;
    accessible(sym, out);
    assign_to(sym, out);
    expr_tree(sym, out);
  }
  out << "}" << std::endl;
}
    
} // namespace catk::symdb
