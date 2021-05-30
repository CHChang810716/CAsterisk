#include "forward_decl.hpp"
#include "type.hpp"

namespace catk::symdb {

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

}