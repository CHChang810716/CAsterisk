#include <catk/syntax/expr_impl.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  if(argc < 2) { return 1; }
  tao::pegtl::file_input<> in(argv[1]);
  auto root = tao::pegtl::parse_tree::parse<RuleFile, catk::syntax::ASTSelector>(in);
  tao::pegtl::parse_tree::print_dot(std::cout, *root);
}