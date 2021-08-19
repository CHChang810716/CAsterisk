#pragma once

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include "ast.hpp"
#include <range/v3/view/drop_last.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

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
> {
  template<class T>
  static auto& literal(T& ast) {
    assert(ast.template is<IntLiteral>());
    return *ast.children.at(0);
  }
  template<class T>
  static auto& tag(T& ast) {
    assert(ast.template is<IntLiteral>());
    return *ast.children.at(1);
  }
  template<class T, class Store>
  static void interpret(T& ast, Store& store) {
    assert(ast.template is<IntLiteral>());
    auto tag_ = tag(ast).content();
    auto content = literal(ast).content();
    if(tag_ == "u8") {
      store = boost::lexical_cast<std::uint8_t>(content);
    } else if(tag_ == "u16") {
      store = boost::lexical_cast<std::uint16_t>(content);
    } else if(tag_ == "u32") {
      store = boost::lexical_cast<std::uint32_t>(content);
    } else if(tag_ == "u64") {
      store = boost::lexical_cast<std::uint64_t>(content);
    } else if(tag_ == "i8") {
      store = boost::lexical_cast<std::int8_t>(content);
    } else if(tag_ == "i16") {
      store = boost::lexical_cast<std::int16_t>(content);
    } else if(tag_ == "i32") {
      store = boost::lexical_cast<std::int32_t>(content);
    } else if(tag_ == "i64") {
      store = boost::lexical_cast<std::int64_t>(content);
    } else {
      assert(0);
    }
  }
};


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
> {
  template<class T>
  static auto& literal(T& ast) {
    assert(ast.template is<FPLiteral>());
    return *ast.children.at(0);
  }
  template<class T>
  static auto& tag(T& ast) {
    assert(ast.template is<FPLiteral>());
    return *ast.children.at(1);
  }
  template<class T, class Store>
  static void interpret(T& ast, Store& store) {
    assert(ast.template is<FPLiteral>());
    auto tag_ = tag(ast).content();
    if(tag_ == "f32") {
      store = std::stof(literal(ast).content());
    } else if (tag_ == "f64") {
      store = std::stod(literal(ast).content());
    }
  } 
};

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
> {
  template<class T>
  static auto& function(T& ast) {
    // TODO: use key word "if" as function ast
  }
};

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
  template<class T>
  static auto& left_id(T& ast) {
    assert(ast.template is<Param>());
    return *ast.children.at(0);
  }
  template<class T>
  static auto& right_expr(T& ast) {
    assert(ast.template is<Param>());
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
struct LambdaLiteral;

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
  template<class T>
  static auto& left_list(T& ast) {
    assert(ast.template is<AssignStmt>());
    return *ast.children.at(0);
  }
  template<class T>
  static auto& right_expr(T& ast) {
    assert(ast.template is<AssignStmt>());
    return *ast.children.at(1);
  }
  template<class T>
  static auto& left_values(T& ast) {
    return ast.children.at(0)->children;
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
struct ContextStmts : tao::pegtl::seq<
  tao::pegtl::star<tao::pegtl::space>,
  tao::pegtl::opt<StmtList>,
  tao::pegtl::star<tao::pegtl::space>,
  RetStmt,
  tao::pegtl::star<tao::pegtl::space>
> {};
struct RetContext : tao::pegtl::seq<
  tao::pegtl::opt<CaptureList>,
  tao::pegtl::star<tao::pegtl::space>,
  tao::pegtl::one<'{'>,
  ContextStmts,
  tao::pegtl::one<'}'>
> {
  template<class T>
  static auto* capture_list(T& ast) {
    assert(ast.template is<RetContext>());
    if(ast.children.at(0)->template is<CaptureList>()) {
      return ast.children.at(0).get();
    } else {
      return (AST*)nullptr;
    }
  }
  template<class T>
  static auto& stmts(T& ast) {
    assert(ast.template is<RetContext>());
    return *ast.children.back();
  }
};
struct LambdaLiteral : tao::pegtl::if_must<
  TAO_PEGTL_KEYWORD("fn"),
  tao::pegtl::star<tao::pegtl::space>,
  SpacePad<tao::pegtl::one<'('>>,
  tao::pegtl::opt<ParamList>,
  SpacePad<tao::pegtl::one<')'>>,
  tao::pegtl::star<tao::pegtl::space>,
  RetContext
> {
  template<class T>
  static auto& params(T& ast) {
    assert(ast.template is<LambdaLiteral>());
    return ast.children.at(0)->children; 
  }
  template<class T>
  static auto& body(T& ast) {
    assert(ast.template is<LambdaLiteral>());
    return *ast.children.back();
  }
  template<class T>
  static auto capture_list(T& ast) {
    assert(ast.template is<LambdaLiteral>());
    return RetContext::capture_list(body(ast));
  }
  template<class T>
  static auto& stmts(T& ast) {
    assert(ast.template is<LambdaLiteral>());
    return RetContext::stmts(body(ast));
  }
};

struct Term : tao::pegtl::sor<
  IfExpr,
  tao::pegtl::seq<tao::pegtl::one<'('>, SpacePad<Expr>, tao::pegtl::one<')'>>,
  RetContext,
  Literal,
  UnaryExpr,
  FCallExpr,
  Identifier
> {
};

struct BinOp : tao::pegtl::sor<
  tao::pegtl::one<'+', '-', '*', '/', '%', '&', '|', '^', '<', '>'>,
  TAO_PEGTL_STRING("=="), TAO_PEGTL_STRING("<="), TAO_PEGTL_STRING(">="), TAO_PEGTL_STRING("!=")
> {};

struct BinExpr : tao::pegtl::seq<Term, tao::pegtl::pad<BinOp, tao::pegtl::space>, Expr> {
  template<class T>
  static auto& function(T& ast) {
    assert(ast.template is<BinExpr>());
    assert(ast.children.size() == 3);
    return ast.children[1];
  }
  template<class T>
  static auto opnds(T& ast) {
    assert(ast.template is<BinExpr>());
    assert(ast.children.size() == 3);
    std::vector<T*> res({
      ast.children[0].get(), 
      ast.children[1].get()
    });
    return res;
  }
};

struct Expr : tao::pegtl::sor<
  BinExpr,
  Term
> {
  template<class T>
  static auto& function(T& ast) {
    assert(ast.template is<Expr>());
    

  }
  template<class T>
  static auto opnds(T& ast) {
    assert(ast.template is<Expr>());

  }
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

    DecIntLiteral,
    HexIntLiteral,
    IntTag,
    IntLiteral, 

    FPLiteralV0,
    FPLiteralV1,
    FPTag,
    FPLiteral,

    Identifier,

    Expr,
    UnaryExpr,
    BinExpr,
    IfExpr,
    FCallExpr,

    RetContext,
    RetStmt,
    ContextStmts,
    AssignStmt,
    CaptureList,
    CaptureItem,
    ParamList,
    Param,
    FCallParamBind,
    AssignLeftHand,
    IgnoredLval
  >
>;

// TODO: bop, if, uop must be a captureable ast

}