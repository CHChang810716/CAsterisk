#pragma once
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <catk/symdb/forward_decl.hpp>
namespace catk::syntax {


struct AST : public tao::pegtl::parse_tree::basic_node<AST> {
  using Base = tao::pegtl::parse_tree::basic_node<AST>;
  using Children = Base::children_t;
  void set_symbol(symdb::Symbol& sym) {
    sym_ = &sym;
  }
  symdb::Symbol& symbol() const { 
    assert(sym_);
    return *sym_; 
  }
private:
  symdb::Symbol* sym_ {nullptr};
};

}