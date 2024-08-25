#pragma once
#include <variant>
#include <llvm/IR/Module.h>

namespace catk::behavior {

using UnitHandle = std::variant<llvm::Module*>;

class Unit {
public:
  template<class T>
  explicit Unit(T&& handle)
  : handle_(std::forward<T>(handle))
  {}
  llvm::Module* as_llvm_module() {
    return std::get<llvm::Module*>(handle_);
  }
private:
  UnitHandle handle_;
}; 
  
} // namespace catk::behavior
