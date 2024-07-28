#include <gtest/gtest.h>
#include <avalon/app/path.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/module.hpp>
#include <fstream>

TEST(semantic_test, file_fib) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  auto f = avalon::app::test_data_dir() / "fib.car";
  tao::pegtl::file_input<> in(f.string());
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  auto* mod = catk::semantics::Module::from_ast(*root);
  mod->dump(catk::io::fmout);
}

TEST(semantic_test, context) {
  using RuleRetCtx = tao::pegtl::must<catk::syntax::RetContext>;
  auto expr_str = "{ ret 1i32; }";
  tao::pegtl::memory_input<> in(expr_str, "");
  auto root = tao::pegtl::parse_tree::parse<
    RuleRetCtx, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  EXPECT_TRUE(root->is_root());
  auto* ctx = catk::semantics::Context::from_ast(*(root->children[0]), {});
  std::stringstream mout;
  catk::io::FmtStream fmout(mout);
  ctx->dump(fmout);
  EXPECT_EQ(mout.str(), "{\n  ret 1i32;\n}");
}

TEST(semantic_test, file_addmul) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  auto f = avalon::app::test_data_dir() / "addmul.car";
  tao::pegtl::file_input<> in(f.string());
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  EXPECT_ANY_THROW(auto* mod = catk::semantics::Module::from_ast(*root));
}