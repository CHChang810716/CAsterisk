#include <catk/syntax.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#include <iostream>
#include <catk/utils.hpp>
// #include <catk/analysis/alloc_sym_dep.h>
// #include <catk/symdb.hpp>
#include <fstream>
int main(int argc, char *argv[]) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  if(argc < 2) { return 1; }
  tao::pegtl::file_input<> in(argv[1]);
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  std::ofstream ast_dot{"ast.dot"};
  std::ofstream alloc_sym_dot{"alloc_sym.dot"};
  catk::print_dot(ast_dot, *root);
  // catk::analysis::alloc_sym_dep::root(*root);
  // catk::semantics::sym_graph(catk::get_sym_db(), alloc_sym_dot);

}