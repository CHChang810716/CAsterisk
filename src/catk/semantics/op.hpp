#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <catk/syntax/ast.hpp>
#include <avalon/debug_id.hpp>

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