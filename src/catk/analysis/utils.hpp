#pragma once
#include <functional>
#include <catk/syntax/ast.hpp>
#include <catk/symdb/symbol.hpp>
namespace catk::analysis {

using ResolverFunctor 
  = std::function<void(const catk::syntax::AST&, symdb::Symbol*)>;

}