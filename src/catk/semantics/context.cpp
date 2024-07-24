#include <cstdint>
#include <catk/syntax/ast.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/semantics/memory.hpp>
#include <unordered_set>

namespace catk::semantics {

Context* Context::from_ast(catk::syntax::AST& ast, const std::vector<catk::syntax::AST*>& params) {
  // make context object and function
  Context& ctx = DB::get().alloc<Context>();
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
  if (ast.is<catk::syntax::File>()) {
    pstmts = &catk::syntax::File::stmts(ast);
  } else if (ast.is<catk::syntax::RetContext>()) {
    // handle capture symbol and parameter symbol
    // set pstmts
    auto* pcl_ast = catk::syntax::RetContext::capture_list(ast);
    if (pcl_ast) {
      auto& cl_ast = *pcl_ast;
      for (auto& pcitem : cl_ast.children) {
        Symbol* csym = guard.last->accessible_.at(pcitem->string());
        ctx.accessible_[csym->get_name()] = csym;
        ctx.captures_.push_back(csym);
      }
    }
    pstmts = &catk::syntax::RetContext::stmts(ast);
  } else {
    rt_assert(false, "unknown context ast: " + ast.content());
  }
  for(auto& pparam : params) {
    auto& param = *pparam;
    auto& id_ast = catk::syntax::Param::left_id(param);
    Symbol *param_sym = Symbol::from_ast(param);
    ctx.accessible_[id_ast.string()] = param_sym;
    ctx.params_.push_back(param_sym);
  }
  for(auto&& pstmt : pstmts->children) {
    catk::syntax::AST& stmt = *pstmt;
    rt_assert(is_context_stmt(stmt), "must be context statement");
    if (stmt.is<syntax::AssignStmt>()) {
      auto name = Symbol::get_name_from_assign_stmt(stmt);
      auto*& sym = ctx.accessible_[name];
      if (sym) continue;
      sym = Symbol::from_ast(stmt);
      rt_assert(sym != nullptr, "Symbol parse error: " + stmt.content());
    } else if (stmt.is<syntax::RetStmt>()) {
      ctx.ret_expr_ = RetExpr::from_ast(stmt);
    }
  }
  rt_assert(ctx.ret_expr_, "context must have return statement");
  return &ctx;
}

void Context::dump(std::ostream& out) const {
  std::unordered_set<Symbol*> caps;
  std::unordered_set<Symbol*> params;
  if (!captures_.empty()) {
    out << '[';
    out << captures_[0];
    caps.insert(captures_[0]);
    for (int i = 1; i < captures_.size(); ++i) {
      auto& c = captures_[i];
      out << ',' << c->get_name();
      caps.insert(c);
    }
    out << "] ";
  }
  if (!params_.empty()) {
    out << "(";
    out << params_[0];
    params.insert(params_[0]);
    for (int i = 1; i < params_.size(); ++i) {
      auto& c = params_[i];
      out << ',' << c->get_name();
      params.insert(c);
    }
    out << ") ";
  }
  out << "{\n";
  for (auto& [name, sym] : accessible_) {
    sym->dump(out);
  }
  out << "}\n";
}

}