#pragma once
#include <catk/symdb.hpp>
#include <catk/syntax.hpp>

namespace catk::analysis {

constexpr struct InitSymdb {
  void operator()(const syntax::AST& ast) const {
  }
} init_symdb;

}