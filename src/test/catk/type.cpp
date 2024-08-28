#include <gtest/gtest.h>
#include <avalon/app/path.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/module.hpp>
#include <fstream>
#include <catk/type.hpp>
#include <fmt/format.h>

TEST(type_test, file_fib) {
  using namespace catk::semantics;
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  auto f = avalon::app::test_data_dir() / "fib.car";
  tao::pegtl::file_input<> in(f.string());
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  auto* mod = catk::semantics::Module::from_ast(*root);
  EXPECT_EQ(catk::get_type(mod->get_context()->get_symbol("y"))->get_id(), fmt::format("{}_pmt", (int)CATK_INT64));
  EXPECT_EQ(catk::get_type(mod->get_context()->get_symbol("y"))->get_id(), fmt::format("{}_pmt", (int)CATK_INT64));
}
