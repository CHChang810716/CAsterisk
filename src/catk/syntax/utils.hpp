#pragma once
#include <avalon/mpl/type_list.hpp>
#include <catk/syntax/ast.hpp>
#include <cstddef>

namespace catk::syntax {

namespace detail {

template<class TL> 
struct IsInImpl {
  inline static bool res(AST& ast) {
    if (ast.is<avalon::mpl::head<TL>>()) return true;
    return IsInImpl<avalon::mpl::tail<TL>>::res(ast);
  }
};
template<>
struct IsInImpl<avalon::mpl::TypeList<>> {
  inline static bool res(AST& ast) {
    return false;
  }
};

template<class TL, template<class TLE> class Visitor> 
struct VisitImpl {
  inline static decltype(auto) res(AST& ast) {
    using TLE0 = avalon::mpl::head<TL>;
    using TLEn = avalon::mpl::tail<TL>;
    if (ast.is<TLE0>()) {
      return Visitor<TLE0>::eval(ast);
    }
    return VisitImpl<TLEn, Visitor>::res(ast);
  }
};

template<template<class TLE> class Visitor> 
struct VisitImpl<avalon::mpl::TypeList<>, Visitor> {
  inline static decltype(auto) res(AST& ast) {
    return decltype(Visitor<std::nullptr_t>::eval(ast))();
  }
};

} // namespace 

template<class SyntaxElemTypeList>
inline static bool is_in(AST& ast) {
  return detail::IsInImpl<SyntaxElemTypeList>::res(ast);
}

template<class SyntaxElemTypeList, template<class TLE> class Visitor>
inline static decltype(auto) visit(AST& ast) {
  return detail::VisitImpl<SyntaxElemTypeList, Visitor>::res(ast);
}

}