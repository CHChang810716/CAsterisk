#pragma once
#include <vector>
#include <memory>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include "symtable.hpp"

namespace catk::analysis {

struct Context {
  using Ptr = std::shared_ptr<Context>;
  using Children = std::vector<Ptr>;

  std::tuple<bool, Symbol&> get_or_create_symbol(const std::string_view& name) {
    auto iter = symbols_.find(name.data());
    if(iter != symbols_.end()) {
      return {true, iter->second};
    }
    if(parent_) {
      auto out_symbol = parent_->get_or_create_symbol(name);
      if(std::get<0>(out_symbol)) {
        return out_symbol;
      }
    }
    auto res = symbols_.emplace_hint(iter, name.data(), Symbol(name.data()));
    return {false, res->second};
  }
  Symbol* get_symbol(const std::string_view& name) {
    auto iter = symbols_.find(name.data());
    if(iter != symbols_.end()) {
      return &iter->second;
    }
    if(parent_) {
      return parent_->get_symbol(name);
    }
    return nullptr;
  }
  void add_child(Ptr child) {
    child->parent_ = this;
    children_.push_back(child);
  }
private:
  Children  children_               ;
  ASTPtr    ast_ref_    { nullptr } ;
  SymTable  symbols_                ;
  Context*  parent_     { nullptr } ;
};

using ContextPtr = Context::Ptr;

}