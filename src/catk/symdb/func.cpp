#include "func.hpp"

namespace catk::symdb {
static struct {
  void operator()(
    const Symbol* sym, 
    const std::vector<Symbol*>& params
  ) const {
    // TODO: generate function call IR
  }
  void operator()(
    const LRMOp& op, 
    const std::vector<Symbol*>& params
  ) const {
    op.make_invoke(params);
  }
} make_invoke;

static struct {
  void operator()(const Symbol* sym) {
    // TODO: generate function definition from sym->ast and sym->type(should function type)
  }
  void operator()(const LRMOp& op) {
    op.make_def();
  }
} make_def;


void Func::make_invoke(const std::vector<Symbol*>& params) {
  std::visit(catk::symdb::make_invoke, *this, params);
}

void Func::make_def() {
  std::visit(catk::symdb::make_def, *this);
}
  

} // namespace catk::symdb
