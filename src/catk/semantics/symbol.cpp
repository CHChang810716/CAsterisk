#include <catk/syntax/expr.hpp>
#include <catk/semantics/symbol.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/context.hpp>
#include <catk/semantics/memory.hpp>
#include <catk/semantics/symbol.hpp>
#include <catk/utils.hpp>

namespace catk::semantics {

Expr* Symbol::rhs() const {
  return rhs_;
}

std::string Symbol::get_name_from_assign_stmt(catk::syntax::AST& ast) {
  rt_assert(ast.is<catk::syntax::AssignStmt>(), "must assign statement: " + ast.content());
  auto& lhs_syms_ast = catk::syntax::AssignStmt::left_list(ast);
  auto& lhs_syms = lhs_syms_ast.children;
  rt_assert(lhs_syms.size() == 1, "NYI: multiple LHS: " + lhs_syms_ast.content());
  auto& lhs_sym = *lhs_syms.at(0);
  std::string sym_name = lhs_sym.string();
  return sym_name;
}

Symbol* Symbol::from_ast(catk::syntax::AST& ast) {
  // search symbol from current context
  if (ast.is<catk::syntax::Identifier>()) {
    auto* ctx = get_current_context();
    if (ctx) {
      auto *res = ctx->get_symbol(ast.string());
      if (res) return res;
    }
    rt_assert(false, "identifier not defined: " + ast.string());
  }
  Symbol& sym = DB::get().alloc<Symbol>();
  if (ast.is<catk::syntax::Param>()) {
    sym.name_ = catk::syntax::Param::id(ast).string();
  } else {
    sym.name_ = get_name_from_assign_stmt(ast);
    auto& rhs_ast = catk::syntax::AssignStmt::right_expr(ast);
    sym.rhs_ = Expr::from_ast(rhs_ast);
  }
  return &sym;
}

void Symbol::dump(catk::io::FmtStream& out) const {
  out << name_; // << " = ";
  // if (rhs_) {
  //   out << " = ";
  //   rhs_->dump(out);
  // }
  out.flush();
}

std::vector<Expr*> Symbol::dependencies() const {
  if (!rhs_) return {};
  return {rhs_};
}

Expr* Symbol::clone() const {
  return clone_impl<Symbol>();
}

Expr* Symbol::deep_clone(SymbolTable& st) const {
  Symbol* sym = &DB::get().alloc<Symbol>();
  sym->name_ = name_;
  if (rhs_)
    sym->rhs_ = rhs_->deep_clone(st);
  else 
    sym->rhs_ = nullptr;
  return sym;
}

}