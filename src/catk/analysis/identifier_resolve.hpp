#pragma once
#include <catk/syntax/ast.hpp>
#include "context.hpp"
#include <typeinfo>
#include <catk/syntax.hpp>
#include <fmt/format.h>
namespace catk::analysis {

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

constexpr struct IdentifierResolve {
  void operator()(syntax::AST& ast, Context* ctx = nullptr) const {
    assert(ast.is_root() || ctx != nullptr);
    if(ast.is_root()) {
      assert(ctx == nullptr);
      auto ctx_ptr = ast.get_ext<Context>();
      apply_children(ast, ctx_ptr.get());
      return;
    } else if(ast.is<syntax::RetContext>()) {
      auto ctx_ptr = ast.get_ext<Context>();
      apply_children(ast, ctx_ptr.get());
      return;
    } else if(ast.is<syntax::AssignStmt>()) {
      auto& l_hand = ast.children.at(0);
      auto& r_expr = ast.children.at(1);
      for(auto&& lh_ch : l_hand->children) {
        if(lh_ch->is<syntax::IgnoredLval>()) continue;
        if(lh_ch->is<syntax::Identifier>()) {
          auto& ext = lh_ch->set_ext<IdentifierExt>();
          auto&& [found, entry] = ctx->get_or_create_symbol(lh_ch->string());
          if(!found) {
            entry.set_def(*lh_ch);
          } else {
            entry.add_ref(*lh_ch);
          }
          ext.set_symbol(entry);
        }
      }
      apply_children(*r_expr, ctx);
      return;
    } else if(ast.is<syntax::Identifier>()) {
      auto symname = ast.string();
      auto& ext = ast.set_ext<IdentifierExt>();
      auto* entry = ctx->get_symbol(symname);
      if(!entry) {
        throw std::runtime_error(
          fmt::format("identifier: {} use but not defined", symname)
        );
      }
      return;
    } else {
      apply_children(ast, ctx);
      return;
    }
  }
private:
  void apply_children(syntax::AST& ast, Context* ctx) const {
    for(auto& ch : ast.children) {
      this->operator()(*ch, ctx);
    }
  }
} Identifier_resolve;


}