#pragma once
#include <avalon/mpl/type_list.hpp>
#include <avalon/mpl/type_visit.hpp>
#include <catk/syntax/ast.hpp>
#include <cstddef>

namespace catk::syntax {

namespace detail {

template<class T>
struct IsInVisitor {
  inline bool is(const AST& ast) const {
    return ast.is<T>();
  }
  inline bool operator()(const AST& ast) const {
    return true;
  }
  inline bool operator()() const {
    return false;
  }
};

} // namespace 

template<class SyntaxElemTypeList>
inline static bool is_in(AST& ast) {
  return avalon::mpl::visit<SyntaxElemTypeList, detail::IsInVisitor>(ast);
}

template<class SyntaxElemTypeList, template<class TLE> class Visitor>
inline static decltype(auto) visit(AST& ast) {
  return avalon::mpl::visit<SyntaxElemTypeList, Visitor>(ast);
}

}