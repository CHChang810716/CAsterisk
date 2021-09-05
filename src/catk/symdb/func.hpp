#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include "symbol.hpp"

#include "primary_add_helper.hpp"

#include <llvm/IR/TypeBuilder.h>

namespace catk::symdb {

struct IFunc {
  virtual void emit_invoke(
    const std::vector<Symbol*>& params,
    const std::vector<std::string>& labels
  ) const = 0;
  virtual void emit_def() const = 0;
  Type* ret_type() const { return this->type_->getReturnType(); }
private:
  FuncType* type_;
};

struct IPrimaryOp : public IFunc {
  virtual void emit_invoke(
    const std::vector<Symbol*>& params,
    const std::vector<std::string>& labels
  ) const = 0;
  virtual void emit_def() const override {}
};


template<PrimaryTypes op0_t, PrimaryTypes op1_t>
struct PrimaryAdd : public IPrimaryOp {
  PrimaryAdd() 
  : type_(nullptr)
  {

  }
  virtual void emit_invoke(
    const std::vector<Symbol*>& params,
    const std::vector<std::string>& labels
  ) const override {
    assert(labels.size() == 0);
    // TODO: generate llvm add

  }
};

struct UserFunc : public IFunc {
  void emit_invoke(
    const std::vector<Symbol*>& params,
    const std::vector<std::string>& labels
  ) const override;
  void emit_def() const override;
private:
  symdb::Symbol* func_sym_ {nullptr};
};

}