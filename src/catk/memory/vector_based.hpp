#pragma once
#include <vector>
#include <type_traits>

namespace catk::memory {

template<class Elem>
class DB {
  std::vector<Elem*> exprs_;
public:
  template<class T, class... ContrArgs>
  T& alloc(ContrArgs&&... args) {
    assert((std::is_base_of_v<Elem, T>));
    exprs_.push_back(new T(std::forward<ContrArgs>(args)...));
    return *static_cast<T*>(exprs_.back());
  }
  static DB& get() {
    static DB db;
    return db;
  }
};
  
} // namespace catk::memory
