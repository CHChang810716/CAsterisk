#include <gtest/gtest.h>
#include <avalon/app/path.hpp>
#include <catk/analysis.hpp>

using namespace catk::analysis;

TEST(analysis_test, resolve_test0) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  auto f = avalon::app::test_data_dir() / "analysis0.car";
  tao::pegtl::file_input<> in(f.string());
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  catk::analysis::root_resolve(*root);
  auto& sym_db = catk::get_sym_db();
  EXPECT_TRUE(sym_db.at(0).is_module());
  EXPECT_TRUE(sym_db.at(1).is_context());
}
// TEST(analysis_test, symbol_resolve_test) {
//   using RuleFile = tao::pegtl::must<catk::syntax::File>;
//   auto f = avalon::app::test_data_dir() / "analysis0.car";
//   tao::pegtl::file_input<> in(f.string());
//   auto root = tao::pegtl::parse_tree::parse<
//     RuleFile, 
//     catk::syntax::AST, 
//     catk::syntax::ASTSelector
//   >(in);
//   catk::analysis::context_resolve(*root);
//   catk::analysis::Identifier_resolve(*root);
//   auto& ast_ext0 = root->get_ext<catk::analysis::ContextExt>();
//   auto& root_ctx = ast_ext0.get_ctx();
//   EXPECT_NE(root_ctx.get_symbol("a"), nullptr);
//   EXPECT_NE(root_ctx.get_symbol("b"), nullptr);
//   EXPECT_NE(root_ctx.get_symbol("c"), nullptr);
//   EXPECT_NE(root_ctx.get_symbol("r0"), nullptr);
//   EXPECT_NE(root_ctx.get_symbol("r1"), nullptr);
//   EXPECT_NE(root_ctx.get_symbol("addmul"), nullptr);
//   auto& c0 = root_ctx.children()[0];
//   auto& c1 = root_ctx.children()[1];
//   auto& c2 = root_ctx.children()[2];
//   auto& c3 = root_ctx.children()[3];
//   EXPECT_EQ(c0->get_symbol("a"), root_ctx.get_symbol("a"));
//   EXPECT_EQ(c0->get_symbol("b"), root_ctx.get_symbol("b"));
//   EXPECT_EQ(c1->get_symbol("a"), root_ctx.get_symbol("a"));
//   EXPECT_EQ(c2->get_symbol("c"), root_ctx.get_symbol("c"));
//   EXPECT_EQ(c3->get_symbol("c"), root_ctx.get_symbol("c"));
//   EXPECT_NE(c3->get_symbol("a"), root_ctx.get_symbol("a"));
//   EXPECT_NE(c3->get_symbol("b"), root_ctx.get_symbol("b"));
// 
//   ASSERT_EQ(c3->children().size(), 2);
//   auto& c3_0 = c3->children()[0];
//   auto& c3_1 = c3->children()[1];
// 
//   EXPECT_EQ(c3_0->get_symbol("c"), root_ctx.get_symbol("c"));
//   EXPECT_EQ(c3_0->get_symbol("b"), c3->get_symbol("b"));
//   EXPECT_EQ(c3_1->get_symbol("b"), c3->get_symbol("b"));
// }
// TEST(analysis_test, typeindex_test) {
//   TypeIndex ti;
//   auto int32 = ti.add("int32");
//   EXPECT_EQ(int32.name, "int32");
//   EXPECT_EQ(int32.id, 0);
//   auto int64 = ti.add("int64");
//   EXPECT_EQ(int64.name, "int64");
//   EXPECT_EQ(int64.id, 1);
//   auto _double = ti.add("double");
//   EXPECT_EQ(_double.name, "double");
//   EXPECT_EQ(_double.id, 2);
//   auto _float = ti.add("float");
//   EXPECT_EQ(_float.name, "float");
//   EXPECT_EQ(_float.id, 3);
// 
//   auto t0 = ti.get_by_id(2);
//   EXPECT_EQ(t0.name, _double.name);
// 
//   auto t1 = ti.get_by_id(0);
//   EXPECT_EQ(t1.name, int32.name);
// 
//   EXPECT_ANY_THROW(ti.get_by_id(4));
// 
//   auto t2 = ti.get_by_name("double");
//   EXPECT_EQ(t2.id, _double.id);
//   EXPECT_EQ(t2.name, _double.name);
// 
//   EXPECT_ANY_THROW(ti.get_by_name("string"));
// }