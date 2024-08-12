#pragma once
#include <vector>
#include <type_traits>
#include <catk/semantics/expr.hpp>
#include <catk/memory/vector_based.hpp>

namespace catk::semantics {

using DB = catk::memory::DB<Expr>;
// FIXME: bad arch

template<class T>
Expr* Expr::clone_impl() const {
  T* o = &DB::get().alloc<T>();    
  *o = *static_cast<const T*>(this);
  return o;
}

}