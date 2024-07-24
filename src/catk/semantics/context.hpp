#pragma once
#include <catk/syntax/ast.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/syntax/utils.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/utils.hpp>
#include <string_view>
#include <unordered_map>
#include <catk/utils.hpp>

namespace catk::semantics {

using ContextStmtASTs = avalon::mpl::TypeList<
  catk::syntax::AssignStmt,
  catk::syntax::RetStmt
>;

inline static bool is_context_stmt(catk::syntax::AST& stmt) {
  rt_assert(stmt.is<catk::syntax::Statement>(), "must statement");
  return catk::syntax::is_in<ContextStmtASTs>(stmt);
}

class Context : public Expr {
  std::unordered_map<std::string, Symbol*> accessible_;
  std::vector<Symbol*> params_;
  std::vector<Symbol*> captures_;
  RetExpr* ret_expr_;
  static void set_current_context(Context* ctx);
public:
  RetExpr* get_return() const;
  Symbol* get_symbol(const std::string& name) {
    auto itr = accessible_.find(name);
    if (itr == accessible_.end()) {
      return nullptr;
    } else {
      return itr->second;
    }
  }
  virtual void dump(std::ostream& out) const;
  static Context* from_ast(catk::syntax::AST& ast, const std::vector<catk::syntax::AST*>& params);
  
};

}