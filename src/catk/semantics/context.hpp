#pragma once
#include <catk/syntax/ast.hpp>
#include <catk/syntax/expr.hpp>
#include <catk/syntax/utils.hpp>
#include <catk/semantics/expr.hpp>
#include <catk/semantics/utils.hpp>
#include <string_view>
#include <unordered_map>
#include <catk/io/fmt_stream.hpp>
#include <catk/utils.hpp>
#include <catk/semantics/symbol.hpp>
#include <catk/semantics/ret_expr.hpp>

namespace catk::semantics {

using ContextStmtASTs = avalon::mpl::TypeList<
  catk::syntax::AssignStmt,
  catk::syntax::RetStmt
>;

inline static bool is_context_stmt(catk::syntax::AST& stmt) {
  return catk::syntax::is_in<ContextStmtASTs>(stmt);
}

class Context : public Expr {
  std::unordered_map<std::string, Symbol*> accessible_;
  std::vector<Symbol*> params_;
  std::vector<Symbol*> captures_;
  std::vector<Symbol*> locals_;
  RetExpr* ret_expr_ {nullptr};
  bool is_immediate_ {true};
  static void set_current_context(Context* ctx);
  void set_immediate(bool f) {
    is_immediate_ = f;
  }
public:
  RetExpr* get_return() const { return ret_expr_; }
  Symbol* get_symbol(const std::string& name) const {
    auto itr = accessible_.find(name);
    if (itr == accessible_.end()) {
      return nullptr;
    } else {
      return itr->second;
    }
  }
  const auto& get_params() const { return params_; }
  virtual void dump(catk::io::FmtStream& out) const;
  virtual std::vector<Expr*> dependencies() const;
  static Context* from_ast(
    catk::syntax::AST& ast, 
    const std::vector<catk::syntax::AST*>& params,
    bool is_immediate = true
  );
  const auto& locals() const { return locals_; }
  bool is_immediate() const {
    return is_immediate_;
  }
  virtual Expr* clone() const;
  virtual Expr* deep_clone(SymbolTable& st) const;
};

}