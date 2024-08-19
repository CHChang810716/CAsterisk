#pragma once
#include <catk/type/type.hpp>
#include <catk/type/memory.hpp>
#include <unordered_map>
namespace catk::type {

struct Context {
  template<class IT>
  Type* getType(IT&& it) {
    TypeId id = serialize_type_id(it);
    auto& ty = internal_[id];
    if (!ty) {
      ty = &DB::get().alloc<Type>(std::forward<IT>(it), std::move(id));
    }
    return ty;
  }
  static Context& get() {
    static Context ctx;
    return ctx;
  }
private:
  std::unordered_map<TypeId, Type*> internal_;
};
  
} // namespace catk::type
