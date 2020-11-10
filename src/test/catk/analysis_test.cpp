#include <catk/analysis/context_resolve.hpp>
#include <catk/analysis/identifier_resolve.hpp>
#include <gtest/gtest.h>
#include <avalon/app/path.hpp>
#include <catk/analysis/typeindex.hpp>

TEST(analysis_test, context_resolve_test) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  auto f = avalon::app::test_data_dir() / "analysis0.car";
  tao::pegtl::file_input<> in(f.string());
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  catk::analysis::context_resolve(*root);
  auto& ast_ext0 = root->get_ext<catk::analysis::ContextExt>();
  auto& root_ctx = ast_ext0.get_ctx();
  ASSERT_EQ(root_ctx.children().size(), 4);
  ASSERT_EQ(root_ctx.children().at(0)->children().size(), 0);
  ASSERT_EQ(root_ctx.children().at(1)->children().size(), 0);
  ASSERT_EQ(root_ctx.children().at(2)->children().size(), 0);
  ASSERT_EQ(root_ctx.children().at(3)->children().size(), 2);
}
TEST(analysis_test, symbol_resolve_test) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  auto f = avalon::app::test_data_dir() / "analysis0.car";
  tao::pegtl::file_input<> in(f.string());
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  catk::analysis::context_resolve(*root);
  catk::analysis::Identifier_resolve(*root);
  auto& ast_ext0 = root->get_ext<catk::analysis::ContextExt>();
  auto& root_ctx = ast_ext0.get_ctx();
  EXPECT_NE(root_ctx.get_symbol("a"), nullptr);
  EXPECT_NE(root_ctx.get_symbol("b"), nullptr);
  EXPECT_NE(root_ctx.get_symbol("c"), nullptr);
  EXPECT_NE(root_ctx.get_symbol("r0"), nullptr);
  EXPECT_NE(root_ctx.get_symbol("r1"), nullptr);
  EXPECT_NE(root_ctx.get_symbol("addmul"), nullptr);
  auto& c0 = root_ctx.children()[0];
  auto& c1 = root_ctx.children()[1];
  auto& c2 = root_ctx.children()[2];
  auto& c3 = root_ctx.children()[3];
  EXPECT_EQ(c0->get_symbol("a"), root_ctx.get_symbol("a"));
  EXPECT_EQ(c0->get_symbol("b"), root_ctx.get_symbol("b"));
  EXPECT_EQ(c1->get_symbol("a"), root_ctx.get_symbol("a"));
  EXPECT_EQ(c2->get_symbol("c"), root_ctx.get_symbol("c"));
  EXPECT_EQ(c3->get_symbol("c"), root_ctx.get_symbol("c"));
  EXPECT_NE(c3->get_symbol("a"), root_ctx.get_symbol("a"));
  EXPECT_NE(c3->get_symbol("b"), root_ctx.get_symbol("b"));

  ASSERT_EQ(c3->children().size(), 2);
  auto& c3_0 = c3->children()[0];
  auto& c3_1 = c3->children()[1];

  EXPECT_EQ(c3_0->get_symbol("c"), root_ctx.get_symbol("c"));
  EXPECT_EQ(c3_0->get_symbol("b"), c3->get_symbol("b"));
  EXPECT_EQ(c3_1->get_symbol("b"), c3->get_symbol("b"));
}