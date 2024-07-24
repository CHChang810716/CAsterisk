#pragma once
#include <deque>
#include <cinttypes>
#include <variant>
#include <string>
#include <tao/pegtl/position.hpp>
#include <fmt/format.h>
namespace catk::semantics {

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
PrimaryTypes operator++(PrimaryTypes& x); 
PrimaryTypes operator*(PrimaryTypes c); 
PrimaryTypes begin(PrimaryTypes r); 
PrimaryTypes end(PrimaryTypes r);

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

static inline std::string get_lambda_name(const tao::pegtl::position& pos) {
  return fmt::format("__lamdba__{}", pos.line);
}
template<class T>
using SharedPtr = std::shared_ptr<T>;

class Context* get_current_context();

}