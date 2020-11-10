#pragma once
#include <catk/syntax/ast.hpp>
#include "context.hpp"
#include <typeinfo>
#include <catk/syntax.hpp>
#include <fmt/format.h>
#include "ast_ext.hpp"
namespace catk::analysis {

constexpr struct IdentifierResolve {
  void operator()(syntax::AST& ast, Context* ctx = nullptr) const {
    // std::cout << (ast.is_root() ? std::string("ROOT") : ast.name()) << std::endl;
    assert(ast.is_root() || ctx != nullptr);
    if(ast.is_root()) {
      assert(ctx == nullptr);
      auto& ctx_ext = ast.get_ext<ContextExt>();
      apply_children(ast, &ctx_ext.get_ctx());
      return;
    } else if(ast.is<syntax::LambdaLiteral>()) {
      auto& lambda_ctx = ast.get_ext<LambdaExt>().get_ctx();
      auto& params = syntax::LambdaLiteral::params(ast);
      for(auto& pm : params) {
        auto& id_ast = syntax::Param::left_id(*pm);
        auto& ext = id_ast.set_ext<IdentifierExt>();
        auto& entry = lambda_ctx.create_symbol(id_ast.string());
        entry.set_def(id_ast);
        ext.set_symbol(entry);
      }
      apply_children(syntax::LambdaLiteral::body(ast), &lambda_ctx);
    } else if(ast.is<syntax::RetContext>()) {
      auto& ctx_ext = ast.get_ext<ContextExt>();
      apply_children(ast, &ctx_ext.get_ctx());
      return;
    } else if(ast.is<syntax::AssignStmt>()) { 
      auto& r_expr = syntax::AssignStmt::right_expr(ast);
      for(auto&& lh_ch : syntax::AssignStmt::left_values(ast)) {
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
      apply_children(r_expr, ctx);
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