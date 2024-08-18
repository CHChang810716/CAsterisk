#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <catk/syntax/ast.hpp>
#include <avalon/debug_id.hpp>
#include <string>
#include <catk/utils.hpp>

namespace catk::semantics {

enum BinOp {
  BOP_ADD,
  BOP_SUB,
  BOP_MUL,
  BOP_DIV,
  BOP_MOD,
  BOP_AND,
  BOP_OR,
  BOP_XOR,
  BOP_LT,
  BOP_GT,
  BOP_EQ,
  BOP_LE,
  BOP_GE,
  BOP_NE
};


enum UnaryOp {
  UOP_ADD,
  UOP_SUB,
  UOP_INV,
  UOP_DEREF,
  UOP_NOT,
  UOP_ADDROF,
};

class IfElseOp {};
class RetOp {};


BinOp ast_str_to_binop(const std::string& str);
UnaryOp ast_str_to_unaryop(const std::string& str);

}
// to_string extension
namespace std {
  inline static std::string to_string(catk::semantics::BinOp bop) {
    using namespace catk::semantics;
    switch(bop) {
      case BOP_ADD: return "+";
      case BOP_SUB: return "-";
      case BOP_MUL: return "*";
      case BOP_DIV: return "/";
      case BOP_MOD: return "%";
      case BOP_AND: return "&";
      case BOP_OR: return "|";
      case BOP_XOR: return "^"; 
      case BOP_LT: return "<";
      case BOP_GT: return ">";
      case BOP_EQ: return "==";
      case BOP_LE: return "<=";
      case BOP_GE: return ">=";
      case BOP_NE: return "!=";
      default: {
        rt_assert(false, "BUG: never be here");
        return "";
      }
    }
  }
  inline static std::string to_string(catk::semantics::UnaryOp uop) {
    using namespace catk::semantics;
    switch(uop) {
      case UOP_ADD: return "+";
      case UOP_SUB: return "-";
      case UOP_INV: return "~";
      case UOP_DEREF: return "*";
      case UOP_NOT: return "!";
      case UOP_ADDROF: return "&";
      default: {
        rt_assert(false, "BUG: never be here");
        return "";
      }
    }
  }
}