#pragma once
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <catk/utils.hpp>

namespace catk::behavior::gen_llvm {

template<class T, class... IRBArgs>
llvm::Type* get_llvm_type(llvm::IRBuilder<IRBArgs...>& irbuilder);

namespace detail {
  
template<class T>
struct CToLLVMType {
  template<class... IRBArgs>
  llvm::Type* operator()(llvm::IRBuilder<IRBArgs...>& irbuilder) const {
    if constexpr(std::is_same_v<T, std::int8_t>) {
      return irbuilder.getInt8Ty();
    } else if(std::is_same_v<T, std::int16_t>) {
      return irbuilder.getInt16Ty();
    } else if(std::is_same_v<T, std::int32_t>) {
      return irbuilder.getInt32Ty();
    } else if(std::is_same_v<T, std::int64_t>) {
      return irbuilder.getInt64Ty();
    } else if(std::is_same_v<T, std::uint8_t>) {
      return irbuilder.getInt8Ty();
    } else if(std::is_same_v<T, std::uint16_t>) {
      return irbuilder.getInt16Ty();
    } else if(std::is_same_v<T, std::uint32_t>) {
      return irbuilder.getInt32Ty();
    } else if(std::is_same_v<T, std::uint64_t>) {
      return irbuilder.getInt64Ty();
    } else {
      rt_assert(false, "unsupport C type for LLVM auto translate");
      return nullptr;
    }
  }
};

template<class T>
struct CToLLVMType<T*> {
  template<class... IRBArgs>
  llvm::Type* operator()(llvm::IRBuilder<IRBArgs...>& irbuilder) const {
    static CToLLVMType<T> helper;
    llvm::Type* x = helper(irbuilder);
    return x->getPointerTo();
  }
};

template<>
struct CToLLVMType<void> {
  template<class... IRBArgs>
  llvm::Type* operator()(llvm::IRBuilder<IRBArgs...>& irbuilder) const {
    return irbuilder.getVoidTy();
  }
};

template<class Ret, class... Args> 
struct CToLLVMType<Ret(Args...)> {

  template<class... IRBArgs>
  llvm::Type* operator()(llvm::IRBuilder<IRBArgs...>& irbuilder) const {
    return llvm::FunctionType::get(
      get_llvm_type<Ret>(irbuilder), 
      {get_llvm_type<Args>(irbuilder)...}, 
      false
    );
  }
private:
  
};
} // namespace detail

template<class T, class... IRBArgs>
llvm::Type* get_llvm_type(llvm::IRBuilder<IRBArgs...>& irbuilder) {
  static detail::CToLLVMType<T> helper;
  return helper(irbuilder);
}

} // namespace catk::behavior::gen_llvm
