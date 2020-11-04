#pragma once
#include <tao/pegtl/contrib/parse_tree.hpp>
namespace catk::syntax {


struct ASTExt {
  virtual const std::type_info& get_rt_type_info() const = 0; 
};


struct AST : public tao::pegtl::parse_tree::basic_node<AST> {
  using Base = tao::pegtl::parse_tree::basic_node<AST>;
  template<class T, class... Args>
  T& set_ext(Args&&... args) {
    assert(ext_ == nullptr);
    T* tmp = new T(std::forward<Args>(args)...);
    ext_.reset(tmp);
    return *tmp;
  }
  template<class T>
  auto get_ext() {
    return std::dynamic_pointer_cast<T>(ext_);
  }
private:
  std::shared_ptr<ASTExt> ext_;
};

}