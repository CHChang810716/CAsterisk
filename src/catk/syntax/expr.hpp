#pragma once

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include "ast.hpp"
#include <range/v3/view/drop_last.hpp>

namespace catk::syntax {

using Identifier = tao::pegtl::identifier;
using IgnoredLval = tao::pegtl::one<'_'>;

template<class R>
using SpacePad = tao::pegtl::pad<R, tao::pegtl::space>;

struct IntTag : tao::pegtl::seq<
  tao::pegtl::opt<tao::pegtl::one<'u'>>, tao::pegtl::one<'i'>, tao::pegtl::sor<
    TAO_PEGTL_STRING("8"), TAO_PEGTL_STRING("16"), TAO_PEGTL_STRING("32"), TAO_PEGTL_STRING("64")
  >
> {};
struct DecIntLiteral : tao::pegtl::seq<
  tao::pegtl::opt<tao::pegtl::one<'-'>>,
  tao::pegtl::plus<tao::pegtl::digit>
> {}; 
struct HexIntLiteral : tao::pegtl::if_must<
  TAO_PEGTL_STRING("0x"), 
  tao::pegtl::plus<tao::pegtl::xdigit>
> {};
struct IntLiteral : tao::pegtl::sor<
  tao::pegtl::seq<DecIntLiteral, IntTag>,
  tao::pegtl::seq<HexIntLiteral, IntTag>
> {};


struct FPTag : tao::pegtl::sor<
  TAO_PEGTL_STRING("f32"), TAO_PEGTL_STRING("f64")
> {};
struct FPLiteralV0 : tao::pegtl::seq<
  tao::pegtl::opt<tao::pegtl::one<'-'>>, tao::pegtl::plus<tao::pegtl::digit>
> {};
struct FPLiteralV1 : tao::pegtl::seq<
  tao::pegtl::opt<tao::pegtl::one<'-'>>, 
  tao::pegtl::star<tao::pegtl::digit>,
  tao::pegtl::one<'.'>,
  tao::pegtl::plus<tao::pegtl::digit>
> {};
struct FPLiteral : tao::pegtl::sor<
  tao::pegtl::seq<FPLiteralV0, FPTag>,
  tao::pegtl::seq<FPLiteralV1, FPTag>
> {};

struct EscapedChar : tao::pegtl::one< '"', '\\', '/', 'b', 'f', 'n', 'r', 't', '0' > {};
struct Escaped : tao::pegtl::sor< EscapedChar /*, unicode */ > {};

struct Char : tao::pegtl::if_then_else< 
  tao::pegtl::one< '\\' >, 
  tao::pegtl::must<Escaped>, 
  tao::pegtl::not_one<'\\'>
> {};
struct StringLiteralContent : tao::pegtl::until<
  tao::pegtl::at< tao::pegtl::one<'"'> >, tao::pegtl::must< Char >
> {};
struct StringLiteral : tao::pegtl::seq<
  tao::pegtl::one<'"'>, 
  tao::pegtl::must< StringLiteralContent >, 
  tao::pegtl::any
> {
  using Content = StringLiteralContent;
};

struct Expr;

struct IfExpr : tao::pegtl::if_must<
  TAO_PEGTL_STRING("if"), 
  SpacePad<tao::pegtl::one<'('>>, 
  Expr,
  SpacePad<tao::pegtl::one<')'>>, 
  Expr,
  SpacePad<TAO_PEGTL_STRING("else")>,
  Expr
> {};

struct ArrayLiteral : tao::pegtl::seq<
  tao::pegtl::one<'['>, 
  tao::pegtl::opt<tao::pegtl::list<Expr, tao::pegtl::one<','>, tao::pegtl::space>>,
  tao::pegtl::one<']'>
> {};

struct Param : tao::pegtl::seq<
  Identifier, 
  tao::pegtl::opt<
    tao::pegtl::seq<SpacePad<tao::pegtl::one<'='>>, Expr>
  >
> {
  static AST& left_id(AST& ast) {
    assert(ast.is<Param>());
    return *ast.children.at(0);
  }
  static AST& right_expr(AST& ast) {
    assert(ast.is<Param>());
    return *ast.children.at(1);
  }

};
struct ParamList : tao::pegtl::list<Param, tao::pegtl::one<','>, tao::pegtl::space> {};

struct CaptureItem : Identifier {};

struct CaptureList : tao::pegtl::seq<
  tao::pegtl::one<'['>,
  tao::pegtl::star<tao::pegtl::space>,
  tao::pegtl::opt<
    tao::pegtl::seq<
      tao::pegtl::list<
        CaptureItem, tao::pegtl::one<','>, tao::pegtl::space
      >,
      tao::pegtl::star<tao::pegtl::space>
    >
  >,
  tao::pegtl::one<']'>
> {};
struct RetContext;
struct LambdaLiteral : tao::pegtl::if_must<
  TAO_PEGTL_KEYWORD("fn"),
  tao::pegtl::star<tao::pegtl::space>,
  SpacePad<tao::pegtl::one<'('>>,
  tao::pegtl::opt<ParamList>,
  SpacePad<tao::pegtl::one<')'>>,
  tao::pegtl::star<tao::pegtl::space>,
  RetContext
> {
  static auto& params(AST& ast) {
    assert(ast.is<LambdaLiteral>());
    return ast.children.at(0)->children; 
  }
  static AST& body(AST& ast) {
    assert(ast.is<LambdaLiteral>());
    return *ast.children.back();
  }
};

struct Literal : tao::pegtl::sor<
  StringLiteral,
  LambdaLiteral,
  ArrayLiteral,
  FPLiteral,
  IntLiteral
> {};

struct FCallParamBind : tao::pegtl::seq<
  Identifier, SpacePad<tao::pegtl::one<'='>>, Expr
> {};

struct FCallParamBindList : tao::pegtl::list<FCallParamBind, tao::pegtl::one<','>, tao::pegtl::space> {};

struct FCallExpr : tao::pegtl::seq<
  Identifier,
  SpacePad<tao::pegtl::one<'('>>, 
  tao::pegtl::opt<FCallParamBindList>,
  SpacePad<tao::pegtl::one<')'>>
> {};

struct UnaryOp : tao::pegtl::one<
  '+', '-', '~', '*', '!', '&'
> {};

struct UnaryExpr : tao::pegtl::seq<UnaryOp, Expr> {};

struct AssignLeftHand : tao::pegtl::list<
  tao::pegtl::sor<
    IgnoredLval,
    Identifier
  >, 
  SpacePad<tao::pegtl::one<','>>
> {};

struct AssignStmt : tao::pegtl::seq<
  AssignLeftHand, 
  SpacePad<tao::pegtl::one<'='>>, 
  Expr,
  tao::pegtl::star<tao::pegtl::space>,
  tao::pegtl::one<';'>
> {
  static AST& left_list(AST& as) {
    assert(as.is<AssignStmt>());
    return *as.children.at(0);
  }
  static AST& right_expr(AST& as) {
    assert(as.is<AssignStmt>());
    return *as.children.at(1);
  }
  static auto& left_values(AST& as) {
    return as.children.at(0)->children;
  }
};

struct Statement : tao::pegtl::sor<
  AssignStmt
> {};
struct StmtList : tao::pegtl::list<
  Statement, 
  tao::pegtl::star<tao::pegtl::space>
> {};
struct RetStmt : tao::pegtl::seq<
  TAO_PEGTL_STRING("ret"), tao::pegtl::star<tao::pegtl::space>, 
  Expr, tao::pegtl::star<tao::pegtl::space>, tao::pegtl::one<';'>
> {};
struct RetContext : tao::pegtl::seq<
  tao::pegtl::opt<CaptureList>,
  tao::pegtl::star<tao::pegtl::space>,
  tao::pegtl::one<'{'>,
  tao::pegtl::star<tao::pegtl::space>,
  tao::pegtl::opt<StmtList>,
  tao::pegtl::star<tao::pegtl::space>,
  RetStmt,
  tao::pegtl::star<tao::pegtl::space>,
  tao::pegtl::one<'}'>
> {};

struct Term : tao::pegtl::sor<
  IfExpr,
  tao::pegtl::seq<tao::pegtl::one<'('>, SpacePad<Expr>, tao::pegtl::one<')'>>,
  RetContext,
  Literal,
  UnaryExpr,
  FCallExpr,
  Identifier
> {};

struct BinOp : tao::pegtl::sor<
  tao::pegtl::one<'+', '-', '*', '/', '%', '&', '|', '^', '<', '>'>,
  TAO_PEGTL_STRING("=="), TAO_PEGTL_STRING("<="), TAO_PEGTL_STRING(">="), TAO_PEGTL_STRING("!=")
> {};

struct BinExpr : tao::pegtl::seq<Term, tao::pegtl::pad<BinOp, tao::pegtl::space>, Expr> {};

struct Expr : tao::pegtl::sor<
  BinExpr,
  Term
> {

};

struct File : tao::pegtl::until<
  tao::pegtl::at<tao::pegtl::eof>,
  SpacePad<tao::pegtl::must<Statement>>
> {
};


template<class Rule>
using ASTSelector = tao::pegtl::parse_tree::selector<
  Rule,
  tao::pegtl::parse_tree::store_content::on<
    BinOp, 
    UnaryOp, 
    StringLiteral,
    LambdaLiteral,
    ArrayLiteral,
    FPLiteral,
    IntLiteral, 
    IfExpr,
    UnaryExpr,
    RetContext,
    FCallExpr,
    RetStmt,
    AssignStmt,
    FCallExpr,
    CaptureItem,
    ParamList,
    Param,
    FCallParamBind,
    Expr,
    AssignLeftHand,
    IgnoredLval,
    Identifier
  >
>;

}