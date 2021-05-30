#pragma once
#include "symdb/forward_decl.hpp"
#include "symdb/symbol.hpp"
#include "symdb/type.hpp"
#include <deque>
namespace catk {

template<class Malloc = std::allocator<symdb::Type>>
struct TypeDB : public std::deque<symdb::Type, Malloc>{
  using Base = std::deque<symdb::Type, Malloc>;
  symdb::Type* alloc() {
    Base::push_back(symdb::Type());
    return &Base::back();
  }
};
template<class Malloc>
struct SymDB : public std::deque<symdb::Symbol, Malloc>{
  using Base = std::deque<symdb::Symbol, Malloc>;
  symdb::Symbol* alloc() {
    symdb::Symbol tmp;
    tmp.db = this;
    Base::emplace_back(std::move(tmp));
    return &Base::back();
  }
};

TypeDB<>& get_type_db();
SymDB<>& get_sym_db();

}