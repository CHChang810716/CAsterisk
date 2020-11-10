#pragma once
#include "context.hpp"
#include <catk/syntax.hpp>
#include <iostream>
namespace catk::analysis {

struct ContextExt : public syntax::ASTExt {
  virtual const std::type_info& get_rt_type_info() const {
    return typeid(ContextExt);
  }
  void set_ctx(ContextPtr ctx) {
    ref_ctx_ = std::move(ctx);
  }
  Context& get_ctx() {
    return *ref_ctx_;
  }
private:
  ContextPtr ref_ctx_ {nullptr};
};

struct IdentifierExt : public syntax::ASTExt {
  virtual const std::type_info& get_rt_type_info() const {
    return typeid(IdentifierExt);
  }
  Symbol& get_symbol() {
    assert(symbol_ != nullptr);
    return *symbol_;
  }
  void set_symbol(Symbol& symbol) {
    symbol_ = &symbol;
  }
private:
  Symbol* symbol_ { nullptr };
};


}