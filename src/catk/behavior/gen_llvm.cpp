#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <catk/semantics.hpp>
#include <catk/type.hpp>
#include <catk/behavior/gen_llvm/driver.hpp>

namespace catk::behavior {

std::unique_ptr<llvm::Module> gen_llvm_mod(const catk::semantics::Module* smod) {
  catk::behavior::gen_llvm::Driver driver;
  return driver.translate_module(smod);
}

} // namespace behavior