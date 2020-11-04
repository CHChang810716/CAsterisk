#pragma once
#include "context.hpp"
#include <catk/syntax.hpp>
namespace catk::analysis {

struct ContextExt : public syntax::ASTExt {
  virtual const std::type_info& get_rt_type_info() const {
    return typeid(ContextExt);
  }
  void set_ref(Context& ctx) {
    ref_ctx_ = &ctx;
  }
private:
  Context* ref_ctx_ {nullptr};
};
constexpr struct ContextResolve {
  void operator()(syntax::AST& ast) const {
    impl(ast);
  }
private:
  ContextPtr impl(syntax::AST& ast) const {
    if(ast.is_root() || ast.is<syntax::RetContext>()) {
      ContextPtr ctx(new Context());
      auto& ext = ast.set_ext<ContextExt>();
      ext.set_ref(*ctx.get());
      add_children(*ctx, ast);
      return ctx;
    }
    return nullptr;
  }
  void add_children(Context& parent, syntax::AST& ast) const {
    for(auto&& ch : ast.children) {
      auto ch_ctx = this->impl(*ch);
      if(ch_ctx) {
        parent.add_child(ch_ctx);
      }
    }
  }
} context_resolve;

}