#pragma once
#include "symdb/forward_decl.hpp"
#include "symdb/symbol.hpp"
#include "symdb/type.hpp"
#include "symdb/func.hpp"
#include "symdb/rel_dbg.h"
#include <deque>
namespace catk {

// TODO: probably should use set


// using TypeDBBase = std::unordered_map<std::string, symdb::Type>;
// struct TypeDB : public TypeDBBase {
//   symdb::Type* alloc(const std::string& name) {
//     return &((*this)[name]);
//   }
//   symdb::Type* find(const symdb::PrimaryUnion& v);
// };
struct SymDB : public std::deque<symdb::Symbol>{
  using Base = std::deque<symdb::Symbol>;
  symdb::Symbol* alloc() {
    symdb::Symbol tmp;
    tmp.db = this;
    Base::emplace_back(std::move(tmp));
    return &Base::back();
  }
};

struct FuncDBTrait {
  struct Key {
    std::string                name;
    std::vector<symdb::Type*>  param_types;
    bool operator==(const Key& other) const {
      if(name != other.name) return false;
      if(param_types.size() != other.param_types.size()) return false;
      for(std::size_t i = 0; i < param_types.size(); i ++) {
        if(param_types[i] != other.param_types[i]) return false;
      }
      return true;
    }
  };
  using Value = std::unique_ptr<symdb::IFunc>;
  struct Hasher {
    using argument_type = Key;
    using result_type   = std::size_t;
    result_type operator()(const argument_type& key) const {
      const auto h1 = std::hash<std::string>()(key.name);
      result_type res = h1;
      for(size_t i = 0; i < key.param_types.size(); i ++) {
        auto& t = key.param_types[i];
        const auto hn = std::hash<symdb::Type*>()(t);
        const auto j = i + 1;
        res ^= ((hn << j) | hn >> (sizeof(result_type) * CHAR_BIT - j));
      }
      return res;
    }
  };
  using Alloc = std::allocator<std::pair<Key, Value>>;
  using Map = std::unordered_map<Key, Value, Hasher, std::equal_to<Key>, Alloc>;
};
struct FuncDB : public FuncDBTrait::Map {
  using Base = typename FuncDBTrait::Map;
};

// TypeDB& get_type_db();
SymDB& get_sym_db();
FuncDB& get_func_db();

}