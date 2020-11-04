#pragma once
#include <string>
#include <map>
#include <memory>
#include <string_view>
#include <catk/utils.hpp>

namespace catk::analysis {

struct Symbol {
  Symbol(
    const std::string_view& name,
    const std::string_view& dtype = ""
  ) 
  : name_(name)
  , dtype_(dtype)
  , def_(nullptr)
  , refs_()
  , complete_(false)
  {
    if(!dtype_.empty()) {
      complete_ = true;
    }
  }
  void set_def(syntax::AST& ast) {
    def_ = &ast;
  }
  void add_ref(syntax::AST& ast) {
    refs_.push_back(&ast);
  }
private:
  std::string           name_;
  std::string           dtype_;
  ASTPtr                def_;
  std::vector<ASTPtr>   refs_;
  bool                  complete_;
};
struct SymTable : public std::map<std::string, Symbol> {
  std::tuple<bool, Symbol&> get_or_create(
    const std::string_view& name
  ) {
    auto iter = this->find(name.data());
    if(iter == this->end()) {
      auto res = this->emplace_hint(iter, name.data(), Symbol(
        name.data()
      ));
      return {false, res->second};
    }
    return {true, iter->second};
  }
};

}