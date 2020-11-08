#include <gtest/gtest.h>
#include <catk/syntax.hpp>
#include <avalon/app/path.hpp>

template<class R>
struct Action {
  template<class T>
  static void apply(const T& in, std::string& str) {
    str = in.string();
    // std::cout << str << std::endl;
  }
};

TEST(expr_test, int_literal) {
  using RuleIntLit = tao::pegtl::must<catk::syntax::IntLiteral>;
  using RuleHexIntLit = tao::pegtl::must<catk::syntax::HexIntLiteral>;
  tao::pegtl::memory_input<> hex_0("0xFF87", "");
  tao::pegtl::memory_input<> i_0("0i32", "");
  tao::pegtl::memory_input<> i_1("12345i32", "");
  tao::pegtl::memory_input<> i_2("0xFF87i32", "");
  tao::pegtl::memory_input<> i_3("0xff32i32", "");
  tao::pegtl::memory_input<> i_4("-12345i32", "");
  tao::pegtl::memory_input<> not_i_0("0", "");
  tao::pegtl::memory_input<> not_i_1("i32", "");
  tao::pegtl::memory_input<> not_i_2("0xi32", "");
  tao::pegtl::memory_input<> not_i_3("123i24", "");
  std::string cap;
  tao::pegtl::parse<RuleHexIntLit, Action>(hex_0, cap);
  EXPECT_EQ(cap, "0xFF87");
  tao::pegtl::parse<RuleIntLit, Action>(i_0, cap);
  EXPECT_EQ(cap, "0i32");
  tao::pegtl::parse<RuleIntLit, Action>(i_1, cap);
  EXPECT_EQ(cap, "12345i32");
  tao::pegtl::parse<RuleIntLit, Action>(i_2, cap);
  EXPECT_EQ(cap, "0xFF87i32");
  tao::pegtl::parse<RuleIntLit, Action>(i_3, cap);
  EXPECT_EQ(cap, "0xff32i32");
  tao::pegtl::parse<RuleIntLit, Action>(i_4, cap);
  EXPECT_EQ(cap, "-12345i32");
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleIntLit, Action>(not_i_0, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleIntLit, Action>(not_i_1, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleIntLit, Action>(not_i_2, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleIntLit, Action>(not_i_3, cap)));
}
TEST(expr_test, fp_literal) {
  using RuleFPLit = tao::pegtl::must<catk::syntax::FPLiteral>;
  tao::pegtl::memory_input<> f_0("0f32", "");
  tao::pegtl::memory_input<> f_1("12345f32", "");
  tao::pegtl::memory_input<> f_2("123.45f32", "");
  tao::pegtl::memory_input<> f_3(".45f32", "");
  tao::pegtl::memory_input<> f_4("-.45f32", "");
  tao::pegtl::memory_input<> f_5("-0.45f32", "");
  tao::pegtl::memory_input<> f_6("12345f64", "");

  tao::pegtl::memory_input<> not_f_0("12345f62", "");
  tao::pegtl::memory_input<> not_f_1("f64", "");
  tao::pegtl::memory_input<> not_f_2(".f64", "");
  tao::pegtl::memory_input<> not_f_3(".", "");

  std::string cap;
  tao::pegtl::parse<RuleFPLit, Action>(f_0, cap);
  EXPECT_EQ(cap, "0f32");
  tao::pegtl::parse<RuleFPLit, Action>(f_1, cap);
  EXPECT_EQ(cap, "12345f32");
  tao::pegtl::parse<RuleFPLit, Action>(f_2, cap);
  EXPECT_EQ(cap, "123.45f32");
  tao::pegtl::parse<RuleFPLit, Action>(f_3, cap);
  EXPECT_EQ(cap, ".45f32");
  tao::pegtl::parse<RuleFPLit, Action>(f_4, cap);
  EXPECT_EQ(cap, "-.45f32");
  tao::pegtl::parse<RuleFPLit, Action>(f_5, cap);
  EXPECT_EQ(cap, "-0.45f32");
  tao::pegtl::parse<RuleFPLit, Action>(f_6, cap);
  EXPECT_EQ(cap, "12345f64");

  EXPECT_ANY_THROW((tao::pegtl::parse<RuleFPLit, Action>(not_f_0, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleFPLit, Action>(not_f_1, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleFPLit, Action>(not_f_2, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleFPLit, Action>(not_f_3, cap)));
}

TEST(expr_test, str_literal) {
  using RuleStrLit = tao::pegtl::must<catk::syntax::StringLiteral>;
  tao::pegtl::memory_input<> str_0("\"asdf123\"", "");
  tao::pegtl::memory_input<> str_1("\"\\n\"", "");
  tao::pegtl::memory_input<> str_2("\"asdf\n123\"", "");
  tao::pegtl::memory_input<> str_3("\"\\0\"", "");

  tao::pegtl::memory_input<> not_str_1("\"\\a\"", "");

  std::string cap;
  tao::pegtl::parse<RuleStrLit, Action>(str_0, cap);
  EXPECT_EQ(cap, "\"asdf123\"");
  tao::pegtl::parse<RuleStrLit, Action>(str_1, cap);
  EXPECT_EQ(cap, "\"\\n\"");
  tao::pegtl::parse<RuleStrLit, Action>(str_2, cap);
  EXPECT_EQ(cap, "\"asdf\n123\"");
  tao::pegtl::parse<RuleStrLit, Action>(str_3, cap);
  EXPECT_EQ(cap, "\"\\0\"");

  EXPECT_ANY_THROW((tao::pegtl::parse<RuleStrLit, Action>(not_str_1, cap)));
}

TEST(expr_test, array_literal) {
  using RuleArrLit = tao::pegtl::must<catk::syntax::ArrayLiteral>;

  tao::pegtl::memory_input<> arr_0("[]", "");
  tao::pegtl::memory_input<> arr_1("[0i32, 1i32, 3i32]", "");
  tao::pegtl::memory_input<> arr_2("[\"a\", \"b\"]", "");
  tao::pegtl::memory_input<> arr_3("[[[]], []]", "");

  tao::pegtl::memory_input<> not_arr_1("[", "");
  tao::pegtl::memory_input<> not_arr_2("]", "");
  tao::pegtl::memory_input<> not_arr_3("[[]", "");
  tao::pegtl::memory_input<> not_arr_4("[0i32", "");

  std::string cap;
  tao::pegtl::parse<RuleArrLit, Action>(arr_0, cap);
  EXPECT_EQ(cap, "[]");
  tao::pegtl::parse<RuleArrLit, Action>(arr_1, cap);
  EXPECT_EQ(cap, "[0i32, 1i32, 3i32]");
  tao::pegtl::parse<RuleArrLit, Action>(arr_2, cap);
  EXPECT_EQ(cap, "[\"a\", \"b\"]");
  tao::pegtl::parse<RuleArrLit, Action>(arr_3, cap);
  EXPECT_EQ(cap, "[[[]], []]");

  EXPECT_ANY_THROW((tao::pegtl::parse<RuleArrLit, Action>(not_arr_1, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleArrLit, Action>(not_arr_2, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleArrLit, Action>(not_arr_3, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleArrLit, Action>(not_arr_4, cap)));

}

TEST(expr_test, expr) {
  using RuleExpr = tao::pegtl::must<catk::syntax::Expr>;

  tao::pegtl::memory_input<> expr_0("asdf", "");
  tao::pegtl::memory_input<> expr_1("\"asdf\"", "");
  tao::pegtl::memory_input<> expr_2(".32f64", "");
  tao::pegtl::memory_input<> expr_3("~3i64", "");
  tao::pegtl::memory_input<> expr_4("4i32 + 5i64", "");

  tao::pegtl::memory_input<> not_expr_0("~ 3i32", "");
  tao::pegtl::memory_input<> not_expr_1("*", "");

  std::string cap;
  tao::pegtl::parse<RuleExpr, Action>(expr_0, cap);
  EXPECT_EQ(cap, "asdf");
  tao::pegtl::parse<RuleExpr, Action>(expr_1, cap);
  EXPECT_EQ(cap, "\"asdf\"");
  tao::pegtl::parse<RuleExpr, Action>(expr_2, cap);
  EXPECT_EQ(cap, ".32f64");
  tao::pegtl::parse<RuleExpr, Action>(expr_3, cap);
  EXPECT_EQ(cap, "~3i64");
  tao::pegtl::parse<RuleExpr, Action>(expr_4, cap);
  EXPECT_EQ(cap, "4i32 + 5i64");

  EXPECT_ANY_THROW((tao::pegtl::parse<RuleExpr, Action>(not_expr_0, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleExpr, Action>(not_expr_1, cap)));
}

TEST(expr_test, ifexpr) {
  using RuleIfExpr = tao::pegtl::must<catk::syntax::IfExpr>;
  auto expr_0_str = "if( cond ) case_t else case_f";
  auto expr_1_str = "if( a == b ) a + 1i32 else b + 1i32";
  auto expr_2_str = "if( b + 1i32 ) ~2i32 else y == 3i32";
  tao::pegtl::memory_input<> expr_0(expr_0_str, "");
  tao::pegtl::memory_input<> expr_1(expr_1_str, "");
  tao::pegtl::memory_input<> expr_2(expr_2_str, "");

  tao::pegtl::memory_input<> not_expr_0("if( )", "");
  tao::pegtl::memory_input<> not_expr_1("if(true) x else", "");

  std::string cap;
  tao::pegtl::parse<RuleIfExpr, Action>(expr_0, cap);
  EXPECT_EQ(cap, expr_0_str);
  tao::pegtl::parse<RuleIfExpr, Action>(expr_1, cap);
  EXPECT_EQ(cap, expr_1_str);
  tao::pegtl::parse<RuleIfExpr, Action>(expr_2, cap);
  EXPECT_EQ(cap, expr_2_str);

  EXPECT_ANY_THROW((tao::pegtl::parse<RuleIfExpr, Action>(not_expr_0, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleIfExpr, Action>(not_expr_1, cap)));
}

TEST(expr_test, ret_context) {
  using RuleRetCtx = tao::pegtl::must<catk::syntax::RetContext>;
  auto expr_0_str = "{ ret 0i32; }";
  auto expr_1_str = "{ ret if( a == b ) a + 1i32 else b + 1i32; }";
  auto expr_2_str = "{ ret { ret 1i32; }; }";
  tao::pegtl::memory_input<> expr_0(expr_0_str, "");
  tao::pegtl::memory_input<> expr_1(expr_1_str, "");
  tao::pegtl::memory_input<> expr_2(expr_2_str, "");

  tao::pegtl::memory_input<> not_expr_0("{}", "");
  tao::pegtl::memory_input<> not_expr_1("{", "");

  std::string cap;
  tao::pegtl::parse<RuleRetCtx, Action>(expr_0, cap);
  EXPECT_EQ(cap, expr_0_str);
  tao::pegtl::parse<RuleRetCtx, Action>(expr_1, cap);
  EXPECT_EQ(cap, expr_1_str);
  tao::pegtl::parse<RuleRetCtx, Action>(expr_2, cap);
  EXPECT_EQ(cap, expr_2_str);

  EXPECT_ANY_THROW((tao::pegtl::parse<RuleRetCtx, Action>(not_expr_0, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleRetCtx, Action>(not_expr_1, cap)));
}

TEST(expr_test, def_func_test) {
  using RuleLambdaExpr = tao::pegtl::must<catk::syntax::LambdaLiteral>;
  auto expr_0_str = "fn () { ret 0i32; }";
  auto expr_1_str = "fn (a) { ret if(a) \"str\" else \"qqq\"; }" ;
  auto expr_2_str = "fn (x, y) { ret x; }";
  auto expr_3_str = "fn (x, y) [m, n] { ret x; }";
  tao::pegtl::memory_input<> expr_0(expr_0_str, "");
  tao::pegtl::memory_input<> expr_1(expr_1_str, "");
  tao::pegtl::memory_input<> expr_2(expr_2_str, "");
  tao::pegtl::memory_input<> expr_3(expr_3_str, "");

  tao::pegtl::memory_input<> not_expr_0("fn", "");
  tao::pegtl::memory_input<> not_expr_1("fn {}", "");

  std::string cap;
  tao::pegtl::parse<RuleLambdaExpr, Action>(expr_0, cap);
  EXPECT_EQ(cap, expr_0_str);
  tao::pegtl::parse<RuleLambdaExpr, Action>(expr_1, cap);
  EXPECT_EQ(cap, expr_1_str);
  tao::pegtl::parse<RuleLambdaExpr, Action>(expr_2, cap);
  EXPECT_EQ(cap, expr_2_str);
  tao::pegtl::parse<RuleLambdaExpr, Action>(expr_3, cap);
  EXPECT_EQ(cap, expr_3_str);

  EXPECT_ANY_THROW((tao::pegtl::parse<RuleLambdaExpr, Action>(not_expr_0, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleLambdaExpr, Action>(not_expr_1, cap)));
}

TEST(stmt_test, basic_test) {
  using RuleStmt = tao::pegtl::must<catk::syntax::Statement>;
  auto stmt_0_str = "a = 3i32;";
  auto stmt_1_str = "a = foo();";
  auto stmt_2_str = "foo = fn () { ret 0i32; };";
  auto stmt_3_str = "foo = fn () [m, n] { ret if(m) n else m; };";
  tao::pegtl::memory_input<> stmt_0(stmt_0_str, "");
  tao::pegtl::memory_input<> stmt_1(stmt_1_str, "");
  tao::pegtl::memory_input<> stmt_2(stmt_2_str, "");
  tao::pegtl::memory_input<> stmt_3(stmt_3_str, "");

  tao::pegtl::memory_input<> not_stmt_0("a = 3i32", "");
  tao::pegtl::memory_input<> not_stmt_1("foo();", "");

  std::string cap;
  tao::pegtl::parse<RuleStmt, Action>(stmt_0, cap);
  EXPECT_EQ(cap, stmt_0_str);
  tao::pegtl::parse<RuleStmt, Action>(stmt_1, cap);
  EXPECT_EQ(cap, stmt_1_str);
  tao::pegtl::parse<RuleStmt, Action>(stmt_2, cap);
  EXPECT_EQ(cap, stmt_2_str);
  tao::pegtl::parse<RuleStmt, Action>(stmt_3, cap);
  EXPECT_EQ(cap, stmt_3_str);

  EXPECT_ANY_THROW((tao::pegtl::parse<RuleStmt, Action>(not_stmt_0, cap)));
  EXPECT_ANY_THROW((tao::pegtl::parse<RuleStmt, Action>(not_stmt_1, cap)));

}

TEST(file_test, addmul) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  auto f = avalon::app::test_data_dir() / "addmul.car";
  tao::pegtl::file_input<> in(f.string());
  std::string cap;
  tao::pegtl::parse<RuleFile, Action>(in, cap);
}