#pragma once
namespace avalon::mpl {

template<class... T>
struct TypeList {};

namespace detail {

template<class TL, class T> 
struct PushBackImpl {};

template<class... TLE, class T> 
struct PushBackImpl<TypeList<TLE...>, T> {
  using Res = TypeList<TLE..., T>;
};

template<class TL> struct HeadImpl {};
template<class T0, class... Tn>
struct HeadImpl<TypeList<T0, Tn...>> {
  using Res = T0;
};

template<class TL> struct TailImpl {};

template<class T0, class... Tn>
struct TailImpl<TypeList<T0, Tn...>> {
  using Res = TypeList<Tn...>;
};

}

template<class TL, class T>
using push_back = typename detail::PushBackImpl<TL, T>::Res;

template<class TL>
using head = typename detail::HeadImpl<TL>::Res;

template<class TL>
using tail = typename detail::TailImpl<TL>::Res;

}