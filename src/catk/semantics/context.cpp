#include <cstdint>
#include <catk/syntax/ast.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/memory.hpp>
#include <catk/io/fmt_stream.hpp>
#include <unordered_set>
#include <iostream>

namespace catk::semantics {


static inline std::string get_ctx_id(
  const catk::syntax::AST& ast 
) {
  auto pos = ast.begin();
  return fmt::format("ctx{}", pos.line);
}
Context* Context::from_ast(
  catk::syntax::AST& ast, 
  const std::vector<catk::syntax::AST*>& params, 
  bool is_immediate
) {
  // make context object and function
  Context& ctx = DB::get().alloc<Context>();
  ctx.set_immediate(is_immediate);
  ctx.set_id(get_ctx_id(ast));
  const struct ContextSwitchGuard {
    Context* last;
    ContextSwitchGuard(Context* curr) {
      last = get_current_context();
      Context::set_current_context(curr);
    }
    ~ContextSwitchGuard() {
      Context::set_current_context(last);
    }
  } guard{&ctx};
  catk::syntax::AST* pstmts = nullptr;
  if (ast.is_root()) {
    pstmts = ast.children[0].get();
  } else if (ast.is<catk::syntax::RetContext>()) {
    // handle capture symbol and parameter symbol
    // set pstmts
    auto* pcl_ast = catk::syntax::RetContext::capture_list(ast);
    if (pcl_ast) {
      auto& cl_ast = *pcl_ast;
      for (auto& pcitem : cl_ast.children) {
        Symbol* csym = guard.last->accessible_.at(pcitem->string());
        csym = &DB::get().alloc<Symbol>(csym, csym->get_name());
        ctx.accessible_[csym->get_name()] = csym;
        ctx.captures_.push_back(csym);
      }
    }
    pstmts = &catk::syntax::RetContext::stmts(ast);
  } else {
    rt_assert(false, fmt::format("unknown context ast: <{}> {}", ast.name(), ast.has_content() ? ast.content() : ""));
  }
  for(auto& pparam : params) {
    auto& param = *pparam;
    rt_assert(!catk::syntax::Param::has_default_expr(param), 
      "not yet support parameter default value");
    auto& id_ast = catk::syntax::Param::id(param);
    Symbol *param_sym = Symbol::from_ast(param);
    ctx.accessible_[id_ast.string()] = param_sym;
    // TODO: default handler
    ctx.params_.push_back(param_sym);
  }
  for(auto&& pstmt : pstmts->children) {
    catk::syntax::AST& stmt = *pstmt;
    // std::cout << stmt.content() << std::endl;
    rt_assert(is_context_stmt(stmt), fmt::format("must be context statement: <{}> {}", stmt.name(), stmt.content()));
    if (stmt.is<syntax::AssignStmt>()) {
      auto name = Symbol::get_name_from_assign_stmt(stmt);
      auto*& sym = ctx.accessible_[name];
      if (sym) continue;
      sym = Symbol::from_ast(stmt);
      ctx.locals_.push_back(sym);
      rt_assert(sym != nullptr, fmt::format("Symbol analyze failed: <{}> {}", stmt.name(), stmt.content()));
    } else if (stmt.is<syntax::RetStmt>()) {
      ctx.ret_expr_ = RetExpr::from_ast(stmt);
    }
  }
  rt_assert(ctx.ret_expr_, "context must have return statement");
  return &ctx;
}

static void dump_deps(Expr* expr, catk::io::FmtStream& fmout, std::unordered_set<Expr*>& dumped) {
  if (dumped.count(expr)) return;
  if (expr->dependencies().size() == 0) {
    return;
  }
  for (auto& dep : expr->dependencies()) {
    dump_deps(dep, fmout, dumped);
  }
  Symbol* sym = dynamic_cast<Symbol*>(expr);
  if (!sym) return;
  sym->dump(fmout);
  fmout << " = ";
  sym->rhs()->dump(fmout);
  fmout << ";" << std::endl;
  dumped.insert(sym);
}

void Context::dump(catk::io::FmtStream& fmout) const {
  std::unordered_set<Expr*> dumped;
  if (!captures_.empty()) {
    fmout << "[";
    bool is_first = true;
    for (auto& cap : captures_) {
      if (!is_first) {
        fmout << ',';
      } else {
        is_first = false;
      }
      cap->dump(fmout);
      dumped.insert(cap);
    }
    fmout << "] ";
  }
  if (!params_.empty()) {
    fmout << "(";
    bool is_first = true;
    for (auto& param : params_) {
      if (!is_first) {
        fmout << ',';
      } else {
        is_first = false;
      }
      param->dump(fmout);
      dumped.insert(param);
    }
    fmout << ") ";
  }
  fmout << "{" << std::endl;
  fmout << catk::io::add_indent(2);
  // for (auto& [name, sym] : accessible_) {
  //   if (caps.count(sym)) continue;
  //   if (params.count(sym)) continue;
  //   sym->dump(fmout);
  //   fmout << " = ";
  //   sym->rhs()->dump(fmout);
  //   fmout << ";" << std::endl;
  // }
  dump_deps(ret_expr_, fmout, dumped);
  ret_expr_->dump(fmout);
  fmout << ";" << std::endl;
  fmout.flush();
  fmout << catk::io::sub_indent(2);
  fmout << "}";
  fmout.flush();
}

std::vector<Expr*> Context::dependencies() const {
  std::vector<Expr*> res;
  res.resize(captures_.size());
  for(size_t i = 0; i < captures_.size(); ++i) {
    assert(captures_[i]);
    res[i] = captures_[i];
  }
  return res;
}

Expr* Context::clone() const {
  return clone_impl<Context>();
}

Expr* Context::deep_clone(SymbolTable& st) const {
  Context* ctx = &DB::get().alloc<Context>();
  auto& symtab = ctx->accessible_;
  for (auto& p : params_) {
    Symbol* nsym = static_cast<Symbol*>(
      p->deep_clone(symtab)
    );
    ctx->params_.push_back(nsym);
    ctx->accessible_[nsym->get_name()] = nsym;
  }
  for (auto& c : captures_) {
    ctx->captures_.push_back(c);
    ctx->accessible_[c->get_name()] = c;
  }
  for (auto& l : locals_) {
    Symbol* nsym = static_cast<Symbol*>(
      l->deep_clone(symtab)
    );
    ctx->locals_.push_back(nsym);
    ctx->accessible_[nsym->get_name()] = nsym;
  } 
  ctx->ret_expr_ = static_cast<RetExpr*>(
    ret_expr_->deep_clone(symtab)
  );
  ctx->is_immediate_ = is_immediate_;
  // TODO: ID duplicate issue need resolve
  ctx->id_ = id_;
  return ctx;
}

}