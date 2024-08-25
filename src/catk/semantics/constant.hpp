#pragma once
#include <catk/semantics/expr.hpp>

namespace catk::semantics {

class Constant : public Expr {
  PrimaryUnion value_ {0};
public:
  Constant() = default;
  explicit Constant(PrimaryUnion&& v)
  : value_(std::move(v))
  {}
  template<class T>
  constexpr T& get() const {
    return std::get<T>(value_);
  }
  PrimaryType get_type() const;
  auto& get_storage() { return value_; }
  const auto& get_storage() const { return value_; }
  virtual void dump(catk::io::FmtStream& out) const;
  virtual std::vector<Expr*> dependencies() const;
  static Constant* from_ast(catk::syntax::AST& ast);

  virtual Expr* clone() const;
  virtual Expr* deep_clone(SymbolTable& st) const;
};

}