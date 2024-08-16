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
private:
  llvm::Module* get_llvm_unit() {
    return std::get<llvm::Module*>(handle_);
  }
  UnitHandle handle_;
}; 
  
} // namespace catk::behavior
