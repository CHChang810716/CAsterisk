#pragma once
#include <vector>
#include <type_traits>
#include <catk/semantics/expr.hpp>

namespace catk::semantics {

class DB {
  std::vector<Expr*> exprs_;
public:
  template<class T, class... ContrArgs>
  T& alloc(ContrArgs&&... args) {
    assert((std::is_base_of_v<Expr, T>));
    exprs_.push_back(new T(std::forward<ContrArgs>(args)...));
    return *static_cast<T*>(exprs_.back());
  }
  static DB& get() {
    static DB db;
    return db;
  }
};

}