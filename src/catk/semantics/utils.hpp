#pragma once
#include <deque>
#include <cinttypes>
#include <variant>
#include <string>
#include <tao/pegtl/position.hpp>
#include <fmt/format.h>

namespace catk::semantics {
enum PrimaryType : std::uint32_t {
  CATK_BOOL   ,
  CATK_UBOOL   ,
  CATK_INT8   ,
  CATK_UINT8  ,
  CATK_INT16  ,
  CATK_UINT16 ,
  CATK_INT32  ,
  CATK_UINT32 ,
  CATK_INT64  ,
  CATK_UINT64 ,
  CATK_FLOAT32,
  CATK_FLOAT64,
  CATK_STRING ,
  CATK_PT_END
};
inline static bool is_int(PrimaryType t) { 
  return t < CATK_FLOAT32;
}
inline static bool is_unsigned_int(PrimaryType t) { 
  return is_int(t) &&  (t & 1 == 0);
}
inline static bool is_signed_int(PrimaryType t) {
  return is_int(t) && !is_unsigned_int(t);
}
inline static bool is_float(PrimaryType t) {
  return t == CATK_FLOAT32 || t == CATK_FLOAT64;
}
inline static bool is_same_int_signed(PrimaryType t0, PrimaryType t1) {
  if (!(is_int(t0) && is_int(t1))) return false; 
  return (t0 & 1) == (t1 & 1);
}
inline static bool is_same_int_bits(PrimaryType t0, PrimaryType t1) {
  if (!(is_int(t0) && is_int(t1))) return false; 
  return (t0 >> 1) == (t1 >> 1);
}
PrimaryType operator++(PrimaryType& x); 
PrimaryType operator*(PrimaryType c); 
PrimaryType begin(PrimaryType r); 
PrimaryType end(PrimaryType r);

using PrimaryUnion = std::variant<
  std::uint8_t,
  std::uint16_t,
  std::uint32_t,
  std::uint64_t,
  std::int8_t,
  std::int16_t,
  std::int32_t,
  std::int64_t,
  float,
  double,
  std::string
>;
template<class T>
using SharedPtr = std::shared_ptr<T>;

class Context* get_current_context();

}