#pragma once
#include <string>
#include <vector>
#include <catk/setting.hpp>
namespace catk::symdb {
enum PrimaryTypes : std::uint32_t {
  CATK_UINT8     = 0,
  CATK_UINT16    = 1,
  CATK_UINT32    = 2,
  CATK_UINT64    = 3,
  CATK_INT8      = 4,
  CATK_INT16     = 5,
  CATK_INT32     = 6,
  CATK_INT64     = 7,
  CATK_FLOAT32   = 8,
  CATK_FLOAT64   = 9,
  CATK_STRING    = 10,
  CATK_PT_END
};
PrimaryTypes operator++(PrimaryTypes& x) {
    return x = (PrimaryTypes)(std::underlying_type<PrimaryTypes>::type(x) + 1); 
}

PrimaryTypes operator*(PrimaryTypes c) {
    return c;
}

PrimaryTypes begin(PrimaryTypes r) {
    return PrimaryTypes::CATK_UINT8;
}

PrimaryTypes end(PrimaryTypes r) {
    PrimaryTypes l=PrimaryTypes::CATK_PT_END;
    return ++l;
}

const char* primary_type_names[] = {
  "uint8",
  "uint16",
  "uint32",
  "uint64",
  "int8",
  "int16",
  "int32",
  "int64",
  "float32",
  "float64",
  "string"
}; 
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
  : deref         (nullptr)
  , bits          (32)
  , id            (CATK_INT32)
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

  void set_primary(bool b)    { if(b) is_primary_      = 1; else is_primary_      = 0;  }
  void set_pointer(bool b)    { if(b) is_pointer_      = 1; else is_pointer_      = 0;  }
  void set_mutable(bool b)    { if(b) is_mutable_      = 1; else is_mutable_      = 0;  }
  void set_aggregate(bool b)  { if(b) is_aggregate_    = 1; else is_aggregate_    = 0;  }
  void set_complete(bool b)   { if(b) is_complete_     = 1; else is_complete_     = 0;  }

  template<class Str>
  void set_name(Str&& s) { name_ = s; }
  const std::string& name() const { return name_; }

  Type*           deref   ;
  std::uint32_t   bits    ;
  std::uint32_t   id      ;

private:
  bool is_what(const std::int64_t& what) const {
    return what == 1;
  }
  std::int64_t 
    is_primary_     : 1,
    is_pointer_     : 1,
    is_aggregate_   : 1,
    is_mutable_     : 1,
    is_complete_    : 1
  ;
  std::string     name_    ;
};


}