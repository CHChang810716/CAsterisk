#include <catk/behavior/gen_llvm/driver.hpp>

namespace catk::behavior::gen_llvm {

struct TypeTransVis {
  llvm::Type* operator()(const catk::Type::StructType& s_struct_ty) const {
    llvm::SmallVector<llvm::Type*, 4> struct_mem_tys;
    for (auto& smem : s_struct_ty.members) {
      struct_mem_tys.push_back(driver_.translate_type(smem));
    }
    return llvm::StructType::create(struct_mem_tys, s_struct_ty.name);
  }
  llvm::Type* operator()(const catk::Type::PrimaryType& s_pri_ty) const {
    using namespace catk::semantics;
    switch(s_pri_ty) {
    case CATK_UINT8  : return builder_.getInt8Ty();
    case CATK_UINT16 : return builder_.getInt16Ty();
    case CATK_UINT32 : return builder_.getInt32Ty();
    case CATK_UINT64 : return builder_.getInt64Ty();
    case CATK_INT8   : return builder_.getInt8Ty();
    case CATK_INT16  : return builder_.getInt16Ty();
    case CATK_INT32  : return builder_.getInt32Ty();
    case CATK_INT64  : return builder_.getInt64Ty();
    case CATK_FLOAT32: return builder_.getFloatTy();
    case CATK_FLOAT64: return builder_.getDoubleTy();
    case CATK_STRING : 
    default:
      rt_assert(false, "NYI");
      return nullptr;
    }
  }
  llvm::Type* operator()(const catk::Type::LazyContext& s_lazy_ctx_ty) const {
    llvm::SmallVector<llvm::Type*, 4> ctx_struct_mem_tys;
    auto& scaps = s_lazy_ctx_ty->captures();
    for (auto& scap : scaps) {
      auto* scap_tar = dynamic_cast<catk::semantics::Symbol*>(scap->rhs());
      auto* scap_tar_ty = catk::get_type(scap_tar);
      // TODO: currently we only capture by reference
      ctx_struct_mem_tys.push_back(driver_.translate_type(scap_tar_ty)->getPointerTo());
    }
    return llvm::StructType::create(*Driver::get_llvm_context(), ctx_struct_mem_tys, id);
  }
  llvm::Type* operator()(const catk::Type::PointerElem& s_ptr_ty) const {
    return driver_.translate_type(s_ptr_ty)->getPointerTo();
  }
  Driver& driver_;
  llvm::IRBuilder<>& builder_;
  type::TypeId id;
};

llvm::Type* Driver::translate_type(const catk::Type* stype) {
  TypeTransVis vis{*this, *this->builder_, stype->get_id()};
  return stype->visit(vis);
}
}