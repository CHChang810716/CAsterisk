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
#include <unordered_map>

namespace catk::semantics {

class Symbol;

using SymbolTable = std::unordered_map<std::string, Symbol*>;

class Expr : public avalon::DebugID<Expr> {
public:
  static Expr* from_ast(catk::syntax::AST& ast);
  virtual void dump(catk::io::FmtStream& out) const = 0;
  virtual std::vector<Expr*> dependencies() const = 0;

  // semantics APIs
  virtual bool has_name() const { return false; }
  virtual const char* get_name() const { return ""; }
  virtual Expr* clone() const = 0;
  virtual Expr* deep_clone(SymbolTable& st) const = 0;
  virtual Expr* deep_clone(SymbolTable&& st) const {
    return this->deep_clone(st);
  }
  std::string dump_str() const;
protected:
  template<class T>
  Expr* clone_impl() const;
};

}