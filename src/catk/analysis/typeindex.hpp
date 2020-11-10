#pragma once
#include <string>
#include <cinttypes>
#include <vector>
#include <set>
#include <memory>
#include <fmt/format.h>

namespace catk::analysis {

struct Type {
  const std::string   name  ;
  const std::uint64_t id    ; // max number if not found
  bool valid() const { return id < std::numeric_limits<std::uint64_t>::max(); }
};
struct TypeIndex {
  TypeIndex()
  : impl_(new TypeIndexImpl())
  {}
  void add(const std::string_view& type_name) {
    impl_->table_.push_back(type_name.data());
    impl_->index_.emplace(impl_->table_.size() - 1);
  }
  Type get_by_name(const std::string_view& name) const {
    __search_target() = name;
    auto iter = impl_->index_.find(std::numeric_limits<std::size_t>::max());
    if(iter == impl_->index_.end()) 
      throw std::invalid_argument(
        fmt::format("type name: {} not found", name.data())
      ); // return Type { "", std::numeric_limits<std::uint64_t>::max() };
    return Type { std::move(__search_target()), *iter };
  }
  Type get_by_id(const std::uint64_t& id) const {
    auto& name = impl_->table_.at(id);
    return Type {name, id};
  }
private:
  static std::string& __search_target() {
    static thread_local std::string target;
    return target;
  }
  struct TypeIndexImpl {
    TypeIndexImpl()
    : table_()
    , index_(IndexCompare(*this))
    {}
    struct IndexCompare {
      IndexCompare(TypeIndexImpl& ti) 
      : target(ti)
      {}
      bool operator()(const std::size_t& i, const std::size_t& j ) const {
        if(i == std::numeric_limits<std::size_t>::max()) {
          return __search_target() < target.table_[j];
        } else if(j == std::numeric_limits<std::size_t>::max()) {
          return target.table_[i] < __search_target();
        } else {
          return target.table_[i] < target.table_[j];
        }
      }
      TypeIndexImpl& target;  
    };
    std::vector<std::string>            table_;
    std::set<std::size_t, IndexCompare> index_;
  };
  std::shared_ptr<TypeIndexImpl> impl_;
};
  
} // namespace catk::analysis
