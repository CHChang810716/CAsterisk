#include <gtest/gtest.h>
#include <catk/syntax/expr_impl.hpp>

template<class R>
struct Action {
  template<class T>
  static void apply(const T& in, std::string& str) {
    str = in.string();
  }
};

TEST(expr_test, int_literal) {
  using RuleIntLit = tp::must<catk::syntax::IntLiteral>;
  using RuleHexIntLit = tp::must<catk::syntax::HexIntLiteral>;
  tp::memory_input<> hex_0("0xFF87", "");
  tp::memory_input<> i_0("0i32", "");
  tp::memory_input<> i_1("12345i32", "");
  tp::memory_input<> i_2("0xFF87i32", "");
  tp::memory_input<> i_3("0xff32i32", "");
  tp::memory_input<> i_4("-12345i32", "");
  tp::memory_input<> not_i_0("0", "");
  tp::memory_input<> not_i_1("i32", "");
  tp::memory_input<> not_i_2("0xi32", "");
  tp::memory_input<> not_i_3("123i24", "");
  std::string cap;
  tp::parse<RuleHexIntLit, Action>(hex_0, cap);
  EXPECT_EQ(cap, "0xFF87");
  tp::parse<RuleIntLit, Action>(i_0, cap);
  EXPECT_EQ(cap, "0i32");
  tp::parse<RuleIntLit, Action>(i_1, cap);
  EXPECT_EQ(cap, "12345i32");
  tp::parse<RuleIntLit, Action>(i_2, cap);
  EXPECT_EQ(cap, "0xFF87i32");
  tp::parse<RuleIntLit, Action>(i_3, cap);
  EXPECT_EQ(cap, "0xff32i32");
  tp::parse<RuleIntLit, Action>(i_4, cap);
  EXPECT_EQ(cap, "-12345i32");
  EXPECT_ANY_THROW((tp::parse<RuleIntLit, Action>(not_i_0, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleIntLit, Action>(not_i_1, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleIntLit, Action>(not_i_2, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleIntLit, Action>(not_i_3, cap)));
}
TEST(expr_test, fp_literal) {
  using RuleFPLit = tp::must<catk::syntax::FPLiteral>;
  tp::memory_input<> f_0("0f32", "");
  tp::memory_input<> f_1("12345f32", "");
  tp::memory_input<> f_2("123.45f32", "");
  tp::memory_input<> f_3(".45f32", "");
  tp::memory_input<> f_4("-.45f32", "");
  tp::memory_input<> f_5("-0.45f32", "");
  tp::memory_input<> f_6("12345f64", "");

  tp::memory_input<> not_f_0("12345f62", "");
  tp::memory_input<> not_f_1("f64", "");
  tp::memory_input<> not_f_2(".f64", "");
  tp::memory_input<> not_f_3(".", "");

  std::string cap;
  tp::parse<RuleFPLit, Action>(f_0, cap);
  EXPECT_EQ(cap, "0f32");
  tp::parse<RuleFPLit, Action>(f_1, cap);
  EXPECT_EQ(cap, "12345f32");
  tp::parse<RuleFPLit, Action>(f_2, cap);
  EXPECT_EQ(cap, "123.45f32");
  tp::parse<RuleFPLit, Action>(f_3, cap);
  EXPECT_EQ(cap, ".45f32");
  tp::parse<RuleFPLit, Action>(f_4, cap);
  EXPECT_EQ(cap, "-.45f32");
  tp::parse<RuleFPLit, Action>(f_5, cap);
  EXPECT_EQ(cap, "-0.45f32");
  tp::parse<RuleFPLit, Action>(f_6, cap);
  EXPECT_EQ(cap, "12345f64");

  EXPECT_ANY_THROW((tp::parse<RuleFPLit, Action>(not_f_0, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleFPLit, Action>(not_f_1, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleFPLit, Action>(not_f_2, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleFPLit, Action>(not_f_3, cap)));
}

TEST(expr_test, str_literal) {
  using RuleStrLit = tp::must<catk::syntax::StringLiteral>;
  tp::memory_input<> str_0("\"asdf123\"", "");
  tp::memory_input<> str_1("\"\\n\"", "");
  tp::memory_input<> str_2("\"asdf\n123\"", "");
  tp::memory_input<> str_3("\"\\0\"", "");

  tp::memory_input<> not_str_1("\"\\a\"", "");

  std::string cap;
  tp::parse<RuleStrLit, Action>(str_0, cap);
  EXPECT_EQ(cap, "\"asdf123\"");
  tp::parse<RuleStrLit, Action>(str_1, cap);
  EXPECT_EQ(cap, "\"\\n\"");
  tp::parse<RuleStrLit, Action>(str_2, cap);
  EXPECT_EQ(cap, "\"asdf\n123\"");
  tp::parse<RuleStrLit, Action>(str_3, cap);
  EXPECT_EQ(cap, "\"\\0\"");

  EXPECT_ANY_THROW((tp::parse<RuleStrLit, Action>(not_str_1, cap)));
}

TEST(expr_test, array_literal) {
  using RuleArrLit = tp::must<catk::syntax::ArrayLiteral>;

  tp::memory_input<> arr_0("[]", "");
  tp::memory_input<> arr_1("[0i32, 1i32, 3i32]", "");
  tp::memory_input<> arr_2("[\"a\", \"b\"]", "");
  tp::memory_input<> arr_3("[[[]], []]", "");

  tp::memory_input<> not_arr_1("[", "");
  tp::memory_input<> not_arr_2("]", "");
  tp::memory_input<> not_arr_3("[[]", "");
  tp::memory_input<> not_arr_4("[0i32", "");

  std::string cap;
  tp::parse<RuleArrLit, Action>(arr_0, cap);
  EXPECT_EQ(cap, "[]");
  tp::parse<RuleArrLit, Action>(arr_1, cap);
  EXPECT_EQ(cap, "[0i32, 1i32, 3i32]");
  tp::parse<RuleArrLit, Action>(arr_2, cap);
  EXPECT_EQ(cap, "[\"a\", \"b\"]");
  tp::parse<RuleArrLit, Action>(arr_3, cap);
  EXPECT_EQ(cap, "[[[]], []]");

  EXPECT_ANY_THROW((tp::parse<RuleArrLit, Action>(not_arr_1, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleArrLit, Action>(not_arr_2, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleArrLit, Action>(not_arr_3, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleArrLit, Action>(not_arr_4, cap)));

}

TEST(expr_test, expr) {
  using RuleExpr = tp::must<catk::syntax::Expr>;

  tp::memory_input<> expr_0("asdf", "");
  tp::memory_input<> expr_1("\"asdf\"", "");
  tp::memory_input<> expr_2(".32f64", "");
  tp::memory_input<> expr_3("~3i64", "");
  tp::memory_input<> expr_4("4i32 + 5i64", "");

  tp::memory_input<> not_expr_0("~ 3i32", "");
  tp::memory_input<> not_expr_1("*", "");

  std::string cap;
  tp::parse<RuleExpr, Action>(expr_0, cap);
  EXPECT_EQ(cap, "asdf");
  tp::parse<RuleExpr, Action>(expr_1, cap);
  EXPECT_EQ(cap, "\"asdf\"");
  tp::parse<RuleExpr, Action>(expr_2, cap);
  EXPECT_EQ(cap, ".32f64");
  tp::parse<RuleExpr, Action>(expr_3, cap);
  EXPECT_EQ(cap, "~3i64");
  tp::parse<RuleExpr, Action>(expr_4, cap);
  EXPECT_EQ(cap, "4i32 + 5i64");

  EXPECT_ANY_THROW((tp::parse<RuleExpr, Action>(not_expr_0, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleExpr, Action>(not_expr_1, cap)));
}

TEST(expr_test, ifexpr) {
  using RuleIfExpr = tp::must<catk::syntax::IfExpr>;
  auto expr_0_str = "if( cond ) case_t else case_f";
  auto expr_1_str = "if( a == b ) a + 1i32 else b + 1i32";
  auto expr_2_str = "if( b + 1i32 ) ~2i32 else y == 3i32";
  tp::memory_input<> expr_0(expr_0_str, "");
  tp::memory_input<> expr_1(expr_1_str, "");
  tp::memory_input<> expr_2(expr_2_str, "");

  tp::memory_input<> not_expr_0("if( )", "");
  tp::memory_input<> not_expr_1("if(true) x else", "");

  std::string cap;
  tp::parse<RuleIfExpr, Action>(expr_0, cap);
  EXPECT_EQ(cap, expr_0_str);
  tp::parse<RuleIfExpr, Action>(expr_1, cap);
  EXPECT_EQ(cap, expr_1_str);
  tp::parse<RuleIfExpr, Action>(expr_2, cap);
  EXPECT_EQ(cap, expr_2_str);

  EXPECT_ANY_THROW((tp::parse<RuleIfExpr, Action>(not_expr_0, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleIfExpr, Action>(not_expr_1, cap)));
}

TEST(expr_test, ret_context) {
  using RuleRetCtx = tp::must<catk::syntax::RetContext>;
  auto expr_0_str = "{ ret 0i32; }";
  auto expr_1_str = "{ ret if( a == b ) a + 1i32 else b + 1i32; }";
  auto expr_2_str = "{ ret { ret 1i32; }; }";
  tp::memory_input<> expr_0(expr_0_str, "");
  tp::memory_input<> expr_1(expr_1_str, "");
  tp::memory_input<> expr_2(expr_2_str, "");

  tp::memory_input<> not_expr_0("{}", "");
  tp::memory_input<> not_expr_1("{", "");

  std::string cap;
  tp::parse<RuleRetCtx, Action>(expr_0, cap);
  EXPECT_EQ(cap, expr_0_str);
  tp::parse<RuleRetCtx, Action>(expr_1, cap);
  EXPECT_EQ(cap, expr_1_str);
  tp::parse<RuleRetCtx, Action>(expr_2, cap);
  EXPECT_EQ(cap, expr_2_str);

  EXPECT_ANY_THROW((tp::parse<RuleRetCtx, Action>(not_expr_0, cap)));
  EXPECT_ANY_THROW((tp::parse<RuleRetCtx, Action>(not_expr_1, cap)));
}