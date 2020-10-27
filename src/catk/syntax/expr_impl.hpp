#pragma once

#include <tao/pegtl.hpp>

#define TP_STR(str) TAO_PEGTL_STRING(str)

namespace tp = tao::pegtl;

namespace catk::syntax {

template<class R>
using SpacePad = tp::pad<R, tp::space>;

struct IntTag : tp::seq<
  tp::opt<tp::one<'u'>>, tp::one<'i'>, tp::sor<
    TP_STR("8"), TP_STR("16"), TP_STR("32"), TP_STR("64")
  >
> {};
struct DecIntLiteral : tp::seq<
  tp::opt<tp::one<'-'>>,
  tp::plus<tp::digit>
> {}; 
struct HexIntLiteral : tp::if_must<
  TP_STR("0x"), 
  tp::plus<tp::xdigit>
> {};
struct IntLiteral : tp::sor<
  tp::seq<DecIntLiteral, IntTag>,
  tp::seq<HexIntLiteral, IntTag>
> {};


struct FPTag : tp::sor<
  TP_STR("f32"), TP_STR("f64")
> {};
struct FPLiteralV0 : tp::seq<
  tp::opt<tp::one<'-'>>, tp::plus<tp::digit>
> {};
struct FPLiteralV1 : tp::seq<
  tp::opt<tp::one<'-'>>, 
  tp::star<tp::digit>,
  tp::one<'.'>,
  tp::plus<tp::digit>
> {};
struct FPLiteral : tp::sor<
  tp::seq<FPLiteralV0, FPTag>,
  tp::seq<FPLiteralV1, FPTag>
> {};

struct EscapedChar : tp::one< '"', '\\', '/', 'b', 'f', 'n', 'r', 't', '0' > {};
struct Escaped : tp::sor< EscapedChar /*, unicode */ > {};

struct Char : tp::if_then_else< 
  tp::one< '\\' >, 
  tp::must<Escaped>, 
  tp::not_one<'\\'>
> {};
struct StringLiteralContent : tp::until<
  tp::at< tp::one<'"'> >, tp::must< Char >
> {};
struct StringLiteral : tp::seq<
  tp::one<'"'>, 
  tp::must< StringLiteralContent >, 
  tp::any
> {
  using Content = StringLiteralContent;
};

struct Expr;

struct IfExpr : tp::if_must<
  TP_STR("if"), 
  SpacePad<tp::one<'('>>, 
  Expr,
  SpacePad<tp::one<')'>>, 
  Expr,
  SpacePad<TP_STR("else")>,
  Expr
> {};

struct ArrayLiteral : tp::if_must<
  tp::one<'['>, 
  tp::opt<tp::list<Expr, tp::one<','>, tp::space>>,
  tp::one<']'>
> {};

struct Literal : tp::sor<
  ArrayLiteral,
  StringLiteral,
  FPLiteral,
  IntLiteral
> {};

struct UnaryOp : tp::one<
  '+', '-', '~', '*', '!', '&'
> {};

struct UnaryExpr : tp::seq<UnaryOp, Expr> {};

struct Term : tp::sor<
  tp::identifier,
  Literal,
  UnaryExpr,
  tp::seq<tp::one<'('>, SpacePad<Expr>, tp::one<')'>>
> {};

struct BinOp : tp::sor<
  tp::one<'+', '-', '*', '/', '%', '&', '|', '^', '<', '>'>,
  TP_STR("=="), TP_STR("<="), TP_STR(">="), TP_STR("!=")
> {};

struct BinExpr : tp::seq<Term, tp::pad<BinOp, tp::space>, Expr> {};

struct AssignLeftHand : tp::list<
  tp::sor<
    tp::one<'_'>,
    tp::identifier
  >, 
  SpacePad<tp::one<','>>
> {};

struct AssignStmt : tp::seq<
  AssignLeftHand, 
  SpacePad<tp::one<'='>>, 
  Expr,
  tp::space,
  tp::one<';'>
> {};

struct Statement : tp::sor<
  AssignStmt
> {};
struct StmtList : tp::list<
  Statement, 
  tp::opt<tp::space>
> {};

struct RetContext : tp::seq<
  tp::one<'{'>,
  tp::opt<tp::space>,
  tp::opt<StmtList>,
  tp::opt<tp::space>,
  tp::seq<
    TP_STR("ret"), tp::space, 
    Expr, tp::opt<tp::space>, tp::one<';'>
  >,
  tp::opt<tp::space>,
  tp::one<'}'>
> {};
struct Expr : tp::sor<
  RetContext,
  IfExpr,
  BinExpr,
  Term
> {};


}