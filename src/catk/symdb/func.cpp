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
    op.emit_invoke(params);
  }
} emit_invoke;

static struct {
  void operator()(const Symbol* sym) {
    // TODO: generate function definition from sym->ast and sym->type(should function type)
  }
  void operator()(const LRMOp& op) {
    op.emit_def();
  }
} emit_def;

static struct {
  Type* operator()(const Symbol* sym) {
    assert(sym->type->is_function());
    return ((FuncType*)sym->type)->ret;
  }
  Type* operator()(const LRMOp& op) {
    return op.ret_type();
  }
} ret_type;

void Func::emit_invoke(const std::vector<Symbol*>& params) const {
  std::visit([&](const auto& var) {
    catk::symdb::emit_invoke(var, params);
  }, static_cast<const Base&>(*this));
}

void Func::emit_def() const {
  std::visit(catk::symdb::emit_def, static_cast<const Base&>(*this));
}

Type* Func::ret_type() const {
  std::visit(catk::symdb::ret_type, static_cast<const Base&>(*this));
}
  

} // namespace catk::symdb
