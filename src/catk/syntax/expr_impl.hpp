#pragma once

#include <tao/pegtl.hpp>

#define TP_STR(str) TAO_PEGTL_STRING(str)

namespace tp = tao::pegtl;

namespace catk::syntax {

struct IntTag : tp::seq<
  tp::opt<tp::one<'u'>>, tp::one<'i'>, tp::sor<
    TP_STR("8"), TP_STR("16"), TP_STR("32"), TP_STR("64")
  >
> {};
struct DecIntLiteral : tp::seq<
  tp::opt<tp::one<'-'>>,
  tp::plus<tp::digit>
> {}; 
struct HexIntLiteral : tp::seq<
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
}