#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include "symbol.hpp"

namespace catk::symdb {

struct LRMOp {
  FuncType* type;
  void make_invoke(const std::vector<Symbol*>& params) const;
  void make_def() const {}
};
// TODO: think about the different OP behavior

struct Func : public std::variant<Symbol*,LRMOp>{
  void make_invoke(const std::vector<Symbol*>& params);
  void make_def();
};

}