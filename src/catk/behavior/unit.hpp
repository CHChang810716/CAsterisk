#pragma once
#include <variant>
#include <llvm/IR/Module.h>

namespace catk::behavior {

using ModulePtr = std::unique_ptr<llvm::Module>;
using UnitHandle = std::variant<ModulePtr>;

class Unit {
public:
  template<class T>
  explicit Unit(T&& handle)
  : handle_(std::forward<T>(handle))
  {}
  auto* as_llvm_module() {
    return std::get<ModulePtr>(handle_).get();
  }
private:
  UnitHandle handle_;
}; 
  
} // namespace catk::behavior
