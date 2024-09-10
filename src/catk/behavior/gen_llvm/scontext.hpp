#pragma once
#include <catk/semantics.hpp>
#include <llvm/IR/Function.h>
namespace catk::behavior::gen_llvm {

class SContext {
  
  llvm::Function* func_;
  catk::semantics::Context* inter_;
};
  
} // namespace catk::behavior::gen_llvm
