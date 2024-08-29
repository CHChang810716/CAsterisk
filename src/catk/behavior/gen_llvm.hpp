#pragma once
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <catk/semantics.hpp>
namespace catk::behavior {

std::unique_ptr<llvm::Module> gen_llvm_mod(const catk::semantics::Module* cmod);

} // namespace catk::behavior
