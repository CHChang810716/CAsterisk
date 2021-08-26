#include "func.hpp"

namespace catk::symdb {

void UserFunc::emit_invoke(
  const std::vector<Symbol*>& params,
  const std::vector<std::string>& labels
) const{
  // TODO: lookup sexpr content to generate function call
}

void UserFunc::emit_def() const {
  // TODO: lookup func_sym_ to generate llvm def code
}


} // namespace catk::symdb
