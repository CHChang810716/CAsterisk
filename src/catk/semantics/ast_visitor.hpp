#pragma once
#include <catk/syntax/ast.hpp>

namespace catk::semantics::detail {
  
template<class T>
struct AstVisitor {
  inline bool is(catk::syntax::AST& ast )const {
    return ast.is<T>();
  }
};

} // namespace catk::semantics::detail
