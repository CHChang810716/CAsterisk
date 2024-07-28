#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <catk/syntax/ast.hpp>
#include <avalon/debug_id.hpp>
#include <catk/semantics/op.hpp>
#include <catk/semantics/utils.hpp>
#include <catk/io/fmt_stream.hpp>
namespace catk::semantics {

class Expr : public avalon::DebugID<Expr> {
  friend class DB;

public:
  static Expr* from_ast(catk::syntax::AST& ast);
  virtual void dump(catk::io::FmtStream& out) const = 0;
  virtual std::vector<Expr*> dependencies() const = 0;

  // sementic APIs
  virtual bool has_name() const {
    return false;
  }
};

class FunctionalExpr;

class Symbol : public Expr {
  std::string name_ {""};
  Expr* rhs_ {nullptr};
public:
  const char* get_name() const {
    return name_.c_str();
  }
  Expr* rhs() const;
  virtual bool has_name() const { return true; }
  virtual std::vector<Expr*> dependencies() const;
  virtual void dump(catk::io::FmtStream& out) const;
  static Symbol* from_ast(catk::syntax::AST& ast);
  static std::string get_name_from_assign_stmt(catk::syntax::AST& ast);
};

class RetExpr : public Expr {
  Expr* rhs_ {nullptr};
public:
  Expr* rhs() const;
  virtual std::vector<Expr*> dependencies() const;
  virtual void dump(catk::io::FmtStream& out) const;
  static RetExpr* from_ast(catk::syntax::AST& ast);

};

class Constant : public Expr {
  PrimaryUnion value_ {0};
public:
  template<class T>
  constexpr T& get() const {
    return std::get<T>(value_);
  }

  auto& get_storage() { return value_; }
  virtual void dump(catk::io::FmtStream& out) const;
  virtual std::vector<Expr*> dependencies() const;
  static Constant* from_ast(catk::syntax::AST& ast);
};
}