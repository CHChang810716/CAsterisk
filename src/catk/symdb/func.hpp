#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include "symbol.hpp"

namespace catk::symdb {

struct LRMOp {
  FuncType* type;
  virtual void emit_invoke(const std::vector<Symbol*>& params) const;
  virtual void emit_def() const {}
  Type* ret_type() const { return type->ret; }
};

struct LRMAdd : public LRMOp {


};

using FuncBase = std::variant<Symbol*,LRMOp>;

struct Func : public FuncBase {
  using Base = FuncBase;
  void emit_invoke(const std::vector<Symbol*>& params) const;
  void emit_def() const;
  Type* ret_type() const;
};

}