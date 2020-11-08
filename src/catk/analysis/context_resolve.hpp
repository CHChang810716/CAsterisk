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
using LambdaExt = ContextExt;
constexpr struct ContextResolve {
  void operator()(syntax::AST& ast) const {
    impl(ast, nullptr);
  }
private:
  void impl(syntax::AST& ast, Context* pctx) const {
    // std::cout << (ast.is_root() ? std::string("ROOT") : ast.name()) << std::endl;
    if(ast.is_root() || ast.is<syntax::RetContext>()) {
      ContextPtr ctx(new Context());
      auto& ext = ast.set_ext<ContextExt>();
      ext.set_ctx(ctx);
      add_children(*ctx, ast);
      if(pctx) pctx->add_child(std::move(ctx));
    } else if(ast.is<syntax::LambdaLiteral>()) {
      ContextPtr ctx(new Context());
      auto& ext = ast.set_ext<LambdaExt>();
      ext.set_ctx(ctx);
      auto& body = syntax::LambdaLiteral::body(ast);
      add_children(*ctx, body);
      if(pctx) pctx->add_child(std::move(ctx));
    } else {
      add_children(*pctx, ast);
    }
  }
  void add_children(Context& parent, syntax::AST& ast) const {
    for(auto&& ch : ast.children) {
      this->impl(*ch, &parent);
    }
  }
} context_resolve;

}