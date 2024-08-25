#pragma once
#include <avalon/mpl/type_list.hpp>
#include <cstddef>
#include <stdexcept>
#include <typeinfo>

namespace avalon::mpl {

namespace detail {

template<class TL, class Visitor> 
struct VirtVisitImpl {
  using TLE0 = avalon::mpl::head<TL>;
  using TLEn = avalon::mpl::tail<TL>;
  using NextLvl = VirtVisitImpl<TLEn, Visitor&>;

  VirtVisitImpl(Visitor& vis)
  : visitor_(vis)
  , next_lvl_(vis)
  {}

  template<class T>
  inline decltype(auto) visit(T* target) const {
    if (typeid(*target) == typeid(TLE0)) {
      return visitor_(dynamic_cast<TLE0*>(target));
    }
    return next_lvl_.visit(target);
  }
private:
  Visitor& visitor_;
  NextLvl next_lvl_;
};

template<class Visitor> 
struct VirtVisitImpl<avalon::mpl::TypeList<>, Visitor&> {
  VirtVisitImpl(Visitor& vis)
  : visitor_(vis)
  {}
  template<class T>
  inline decltype(auto) visit(T* target) const {
    return visitor_();
  }
private:
  Visitor& visitor_;
};

} // namespace 

template<class TypeList, class Visitor, class Target>
inline static decltype(auto) virt_visit(Visitor&& visitor, Target* target) {
  detail::VirtVisitImpl<TypeList, Visitor&> impl{visitor};
  return impl.visit(target);
}

template<class TypeList, class Visitor, class Target>
inline static decltype(auto) virt_visit(Target* target) {
  Visitor visitor;
  detail::VirtVisitImpl<TypeList, Visitor&> impl{visitor};
  return impl.visit(target);
}

}