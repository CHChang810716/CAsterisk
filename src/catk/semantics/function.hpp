#pragma once
#include <catk/syntax/ast.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/op.hpp>
#include <catk/semantics/symbol.hpp>
#include <variant>
#include <iostream>

namespace catk::semantics {

namespace detail {

using FunctionProto = std::variant<
  Symbol*, // user function
  BinOp,
  UnaryOp,
  IfElseOp
>;

template<class Expr>
struct FunctionFromExprImpl {
  static FunctionProto make(catk::syntax::AST& func_ast) {
    rt_assert(false, "unknown function expr: " + func_ast.content());
  }
};

template<>
struct FunctionFromExprImpl<catk::syntax::BinExpr> {
  static FunctionProto make(catk::syntax::AST& func_ast) {
    return ast_str_to_binop(func_ast.content());
  }
};

template<>
struct FunctionFromExprImpl<catk::syntax::UnaryExpr> {
  static FunctionProto make(catk::syntax::AST& func_ast) {
    return ast_str_to_unaryop(func_ast.content());
  }
};

template<>
struct FunctionFromExprImpl<catk::syntax::IfExpr> {
  static FunctionProto make(catk::syntax::AST& func_ast) {
    return IfElseOp{};
  }
};

template<>
struct FunctionFromExprImpl<catk::syntax::FCallExpr> {
  static FunctionProto make(catk::syntax::AST& func_ast) {
    return Symbol::from_ast(func_ast);
  }
};

// template<>
// struct FunctionFromExprImpl<catk::syntax::RetStmt> {
//   static FunctionProto make(catk::syntax::AST& func_ast) {
//     return RetOp{};
//   }
// };
}

class Function : public detail::FunctionProto {

public:
  // using detail::FunctionProto::FunctionProto;
  explicit Function(detail::FunctionProto&& v)
  : detail::FunctionProto(std::move(v))
  {}
  Function() = default;
  detail::FunctionProto& get_variant() { return *this; }
  const detail::FunctionProto& get_variant() const { return *this; }
  bool is_user_function_call() const {
    return std::holds_alternative<Symbol*>(get_variant());
  }
  template<class Expr>
  static Function from_expr(catk::syntax::AST& func_ast) {
    // std::cout << func_ast.content() << std::endl;
    auto proto = detail::FunctionFromExprImpl<Expr>::make(func_ast);
    return Function{std::move(proto)};
  }
};

}