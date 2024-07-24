#pragma once
#include <tao/pegtl/contrib/parse_tree.hpp>
namespace catk::syntax {


struct AST : public tao::pegtl::parse_tree::basic_node<AST> {
  using Base = tao::pegtl::parse_tree::basic_node<AST>;
  using Children = Base::children_t;
private:
};

}