#pragma once
#include <avalon/mpl/type_list.hpp>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <catk/utils.hpp>

namespace avalon::mpl {

struct VisitNoMatch {};

namespace detail {

template<class T>
T Val();

template<class TL, template<class TLE> class Visitor> 
struct VisitImpl {
  using TLE0 = avalon::mpl::head<TL>;
  using TLEn = avalon::mpl::tail<TL>;
  using ThisLvl = Visitor<TLE0>;
  using NextLvl = VisitImpl<TLEn, Visitor>;

  template<class T>
  inline decltype(auto) visit(T& target) const {
    if (visitor_.is(target)) {
      return visitor_(target);
    }
    return next_lvl_.visit(target);
  }
private:
  ThisLvl visitor_;
  NextLvl next_lvl_;
};

template<template<class TLE> class Visitor> 
struct VisitImpl<avalon::mpl::TypeList<>, Visitor> {
  using ThisLvl = Visitor<VisitNoMatch>;
  template<class T>
  inline decltype(auto) visit(T& target) const {
    return visitor_();
  }
private:
  ThisLvl visitor_;
};

} // namespace 

template<class TypeList, template<class TLE> class Visitor, class Target>
inline static decltype(auto) visit(Target& target) {
  static detail::VisitImpl<TypeList, Visitor> impl;
  return impl.visit(target);
}

}