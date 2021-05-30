#pragma once
#include <deque>
#include <cinttypes>
namespace catk::symdb {

struct Symbol;
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
}
namespace catk {


template<class Malloc = std::allocator<symdb::Symbol>> struct SymDB;

}