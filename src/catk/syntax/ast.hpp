#pragma once
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <catk/symdb/forward_decl.hpp>
namespace catk::syntax {


struct AST : public tao::pegtl::parse_tree::basic_node<AST> {
  using Base = tao::pegtl::parse_tree::basic_node<AST>;
  void set_symbol(const symdb::Symbol& sym) const {
    sym_ = &sym;
  }
private:
  mutable symdb::Symbol const* sym_ {nullptr};
};

}