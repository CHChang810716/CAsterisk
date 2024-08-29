#include <gtest/gtest.h>
#include <avalon/app/path.hpp>
#include <catk/behavior/gen_llvm/driver.hpp>
#include <sstream>
#include <llvm/IR/Module.h>
#include <llvm/ADT/StringRef.h>
#include <catk/behavior/gen_llvm/utils.hpp>
#include <catk/behavior.hpp>
#include <catk/behavior/unit.hpp>

class CatkUnitTest {
  using Driver = catk::behavior::gen_llvm::Driver;
public:
  void operator()(Driver& driver) {
    driver.curr_mod_.reset(
      new llvm::Module("catk_test_mod", *Driver::get_llvm_context())
    );
    auto test_func = llvm::cast<llvm::Function>(driver.curr_mod_->getOrInsertFunction(
      "catk_test_func",
      catk::behavior::gen_llvm::get_llvm_type<void(void)>(*driver.builder_)
    ).getCallee());
    driver.curr_func_ = llvm::cast<llvm::Function>(test_func);
    auto begBB = llvm::BasicBlock::Create(*Driver::get_llvm_context(), "begin", test_func);
    driver.builder_->SetInsertPoint(begBB);
  }
} unit_test_preset;

TEST(behavior, gen_llvm_const_test) {
  catk::behavior::gen_llvm::Driver driver;
  unit_test_preset(driver);
  catk::semantics::Constant constV(5566);
  llvm::Value* c5566 = driver.translate_value(&constV);
  llvm::ConstantInt* C5566;
  EXPECT_NO_THROW(C5566 = llvm::cast<llvm::ConstantInt>(c5566));
  EXPECT_EQ(C5566->getZExtValue(), 5566);
}

TEST(behavior, gen_llvm_symbol_test) {
  catk::behavior::gen_llvm::Driver driver;
  unit_test_preset(driver);
  catk::semantics::Constant constV(5566);
  catk::semantics::Symbol sym(&constV, "c5566");
  llvm::Value* sym5566 = driver.translate_value(&sym);
  llvm::StoreInst* S5566 = nullptr;
  llvm::ConstantInt* C5566 = nullptr;
  llvm::LoadInst* L5566 = llvm::cast<llvm::LoadInst>(sym5566);
  llvm::AllocaInst* A5566 = llvm::cast<llvm::AllocaInst>(L5566->getPointerOperand());
  for (auto&& U : A5566->users()) {
    if (auto S = llvm::dyn_cast<llvm::StoreInst>(U)) {
      S5566 = S;
    }
  }
  EXPECT_NE(S5566, nullptr);
  C5566 = llvm::cast<llvm::ConstantInt>(S5566->getValueOperand());
  EXPECT_EQ(C5566->getZExtValue(), 5566);
}

TEST(behavior, file_fib_test) {
  using RuleFile = tao::pegtl::must<catk::syntax::File>;
  auto f = avalon::app::test_data_dir() / "fib.car";
  tao::pegtl::file_input<> in(f.string());
  auto root = tao::pegtl::parse_tree::parse<
    RuleFile, 
    catk::syntax::AST, 
    catk::syntax::ASTSelector
  >(in);
  auto* mod = catk::semantics::Module::from_ast(*root);
  auto* cu = catk::get_unit_behavior(mod);
  auto* llvm_mod = cu->as_llvm_module();
  EXPECT_NE(llvm_mod->getFunction("fib_impl"), nullptr);
  EXPECT_NE(llvm_mod->getFunction("fib"), nullptr);
  std::string tmp;
  llvm::raw_string_ostream out(tmp);
  llvm_mod->print(out, nullptr);
  std::cout << tmp << std::endl;
  
}