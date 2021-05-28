#pragma once
#include "symdb/symbol.hpp"
#include "symdb/type.hpp"
#include <deque>
namespace catk {

template<class Malloc = std::allocator<symdb::Type>>
struct TypeDB : public std::deque<symdb::Type, Malloc>{
};
template<class Malloc = std::allocator<symdb::Symbol>>
struct SymDB : public std::deque<symdb::Symbol, Malloc>{
};

constexpr auto get_type_db = []() -> auto& {
  static auto typedb = [](){
    TypeDB<> res;
    if(res.size() != 0) {
      throw std::runtime_error(
        "BUG: typedb should be empty at this point."
      );
    }
    for(const auto& ept : symdb::PrimaryTypes()) {
      symdb::Type pt;
      pt.deref  = nullptr;
      pt.bits   = symdb::primary_type_bits(ept);
      pt.id     = ept;
      pt.set_primary(true);
      pt.set_pointer(false);
      pt.set_mutable(false);
      pt.set_aggregate(false);
      pt.set_complete(true);
      pt.set_name(symdb::primary_type_names[ept]);
      res.push_back(pt);
    }
    return res;
  }();
  return typedb;
};

constexpr auto get_sym_db = []() -> auto& {
  static SymDB<> symdb;
  return symdb;
};

}