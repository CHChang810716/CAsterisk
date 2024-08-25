#pragma once
#include <variant>
#include <vector>
#include <catk/semantics/utils.hpp>
#include <string>

namespace catk::type {

using TypeId = std::string;

struct Type;

struct StructTypeContent {
  const std::string name;
  const std::vector<const Type*> members;
};

struct Type {
  using PrimaryType = catk::semantics::PrimaryType;
  using StructType = StructTypeContent;
  using LazyContext = const catk::semantics::Context*;
  using PointerElem = Type*;
  using Content = std::variant<
    StructType, 
    PrimaryType, 
    PointerElem, 
    LazyContext
  >;

  template<class T>
  Type(
    T&& t, 
    const TypeId& id, 
    bool is_mutable = 0, 
    bool is_undecided = 0
  )
  : is_mutable_(is_mutable)
  , is_undecided_(is_undecided)
  , id_(id)
  , content_(std::forward<T>(t))
  {}

  bool is_mutable() const { return is_mutable_; }
  bool is_pointer() const { return std::holds_alternative<PointerElem>(content_); }
  bool is_primary() const { return std::holds_alternative<PrimaryType>(content_); }
  bool is_struct() const { return std::holds_alternative<StructType>(content_); }
  bool is_lazy_context() const { return std::holds_alternative<LazyContext>(content_); }
  bool is_undecided() const { return is_undecided_; }
  bool is_unsigned_int() const {
    rt_assert(is_primary(), "must be primary");
    return std::get<PrimaryType>(content_) < catk::semantics::CATK_INT8;
  }
  bool is_int() const {
    rt_assert(is_primary(), "must be primary");
    return std::get<PrimaryType>(content_) < catk::semantics::CATK_FLOAT32;
  }
  bool is_signed_int() const {
    rt_assert(is_primary(), "must be primary");
    return is_int() && !is_unsigned_int();
  }
  bool is_float() const {
    rt_assert(is_primary(), "must be primary");
    auto& v = std::get<PrimaryType>(content_);
    return v == catk::semantics::CATK_FLOAT32 || v == catk::semantics::CATK_FLOAT32;
  }
  Type* get_pointer_elem_ty() const {
    rt_assert(is_pointer(), "must be pointer");
    return std::get<PointerElem>(content_);
  }
  LazyContext get_lazy_context() {
    rt_assert(is_lazy_context(), "must be lazy context");
    return std::get<LazyContext>(content_);
  }
  const TypeId& get_id() const {
    return id_;
  }
  template<class Vis>
  auto visit(Vis&& vis) const {
    return std::visit(std::forward<Vis>(vis), content_);
  }
  static Type* get_undecided() {
    static Type ut(PrimaryType::CATK_PT_END, "(undecided)", false, true);
    return &ut;
  }
private:
  std::uint8_t 
    is_mutable_ : 1,
    is_undecided_ : 1
  ;
  TypeId id_;
  Content content_;
};
  
struct {
  TypeId operator()(const StructTypeContent& st) const {
    return st.name;
  }
  TypeId operator()(const catk::semantics::PrimaryType& pt) const {
    return fmt::format("{}_pmt", static_cast<std::uint32_t>(pt));
  }
  TypeId operator()(const catk::semantics::Context* ctx) const {
    rt_assert(ctx != nullptr, "cannot be null");
    auto name = fmt::format("{}_ctx", static_cast<const void*>(ctx));
    return name;
  }
  TypeId operator()(const Type* t) const {
    rt_assert(t != nullptr, "cannot be null");
    return fmt::format("{}_ptr", static_cast<const void*>(t));
  }
  TypeId operator()(const catk::semantics::Context* ctx, const std::vector<Type*>& tys) const {
    std::string param_tys;
    for(auto& t : tys) {
      if (!param_tys.empty()) {
        param_tys += ",";
      }
      param_tys += t->get_id();
    }
    return fmt::format("{}({})", (*this)(ctx), param_tys);
  }
} serialize_type_id;

} // namespace catk::type
