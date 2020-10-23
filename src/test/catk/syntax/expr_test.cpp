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