#include <catk/syntax.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#include <iostream>
#include <catk/utils.hpp>
int main(int argc, char *argv[]) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  if(argc < 2) { return 1; }
  tao::pegtl::file_input<> in(argv[1]);
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  catk::print_dot(std::cerr, *root);
  // catk::analysis::context_resolve(*root);
  // catk::analysis::Identifier_resolve(*root);

}