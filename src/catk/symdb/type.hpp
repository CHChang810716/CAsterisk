#pragma once
#include "forward_decl.hpp"
#include <string>
#include <vector>
#include <catk/setting.hpp>
namespace catk::symdb {

extern const char* primary_type_names[]; 

constexpr auto primary_type_bits = [](auto enum_id) {
  static const std::uint8_t primary_type_bits[] = {
    8, 16, 32, 64, 8, 16, 32, 64, 32, 64
  };
  if(enum_id < sizeof(primary_type_bits)) {
    return primary_type_bits[enum_id];
  } else {
    return target_runtime_pointer_bits();
  }

};
struct Type {
  Type() 
  : bits          (32)
  , is_primary_   (1)
  , is_pointer_   (0)
  , is_aggregate_ (0)
  , is_mutable_   (0)
  , is_complete_  (1)
  , name_         (primary_type_names[CATK_INT32])
  {}
  bool is_primary()   const { return is_what(is_primary_);    }
  bool is_pointer()   const { return is_what(is_pointer_);    }
  bool is_mutable()   const { return is_what(is_mutable_);    }
  bool is_aggregate() const { return is_what(is_aggregate_);  }
  bool is_complete()  const { return is_what(is_complete_);  }
  bool is_function()  const { return is_what(is_function_);  }

  void set_primary(bool b)    { if(b) is_primary_      = 0x1; else is_primary_      = 0x0;  }
  void set_pointer(bool b)    { if(b) is_pointer_      = 0x1; else is_pointer_      = 0x0;  }
  void set_mutable(bool b)    { if(b) is_mutable_      = 0x1; else is_mutable_      = 0x0;  }
  void set_aggregate(bool b)  { if(b) is_aggregate_    = 0x1; else is_aggregate_    = 0x0;  }
  void set_complete(bool b)   { if(b) is_complete_     = 0x1; else is_complete_     = 0x0;  }
  void set_function(bool b)   { if(b) is_function_     = 0x1; else is_function_     = 0x0;  }

  template<class Str>
  void set_name(Str&& s) { name_ = s; }
  const std::string& name() const { return name_; }

  std::uint32_t   bits    ;
  // std::uint32_t   id      ;

private:
  bool is_what(const std::int64_t& what) const {
    return what == 0x1;
  }
  std::uint64_t 
    is_primary_     : 1,
    is_pointer_     : 1,
    is_aggregate_   : 1,
    is_mutable_     : 1,
    is_complete_    : 1,
    is_function_    : 1
  ;
  std::string     name_    ;
};

struct PointerType : public Type {
  PointerType(Type* _deref)
  : Type()
  , deref(_deref)
  {
    this->set_pointer(true);
  }
  Type*           deref   ;
};
struct FuncType : public Type {
  FuncType(Type* _ret, std::vector<Type*> _params)
  : Type()
  , ret(_ret)
  , params(std::move(_params))
  {
    this->set_function(true);
  }
  Type* ret;
  std::vector<Type*> params;
};


}