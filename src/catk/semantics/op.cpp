#include <unordered_map>
#include <string>
#include <catk/semantics/op.hpp>

namespace catk::semantics {

BinOp ast_str_to_binop(const std::string& str) {
  static const std::unordered_map<std::string, BinOp> lookup {
    {"+",  BOP_ADD  },
    {"-",  BOP_SUB  },
    {"*",  BOP_MUL  },
    {"/",  BOP_DIV  },
    {"%",  BOP_MOD  },
    {"&",  BOP_AND  },
    {"|",  BOP_OR   },
    {"^",  BOP_XOR  },
    {"<",  BOP_LT   },
    {">",  BOP_GT   },
    {"==", BOP_EQ   },
    {"<=", BOP_LE   },
    {">=", BOP_GE   },
    {"!=", BOP_NE   }
  };
  return lookup.at(str);
}

UnaryOp ast_str_to_unaryop(const std::string& str) {
  static const std::unordered_map<std::string, UnaryOp> lookup {
    {"+", UOP_ADD},
    {"-", UOP_SUB},
    {"~", UOP_INV},
    {"*", UOP_DEREF},
    {"!", UOP_NOT},
    {"&", UOP_ADDROF}
  };
  return lookup.at(str);
}
}