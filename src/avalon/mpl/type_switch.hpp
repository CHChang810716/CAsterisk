#pragma once
#include <tuple>
#include <cstdint>
#include <utility>
namespace avalon::mpl {

template<class T, class Func>
struct CaseItem {
  Func func;
};



template<class TestF, class Switcher, class... CaseItems>
struct SwitcherCase {
  SwitcherCase(TestF&& test_f, CaseItems&&... cases)
  : test_f_(std::forward<TestF>(test_f))
  , case_items_(std::forward<CaseItems>(cases)...)
  {}

  template<class T, class CaseFunc>
  auto case_(
    CaseFunc&& case_func
  ) const {
    return case_impl<T>(
      std::forward<CaseFunc>(case_func),
      std::index_sequence_for<CaseItems...>()
    );
  }


  template<class T, class CaseFunc, std::size_t... case_items_i>
  auto case_impl(
    CaseFunc&& case_func, 
    const std::integer_sequence<std::size_t, case_items_i...>& dummy  
  ) const {
    return SwitcherCase<
      TestF, 
      Switcher, 
      CaseItems...,
      CaseItem<T, CaseFunc> 
    >(
      std::forward<TestF>(test_f_),
      std::get<case_items_i...>(case_items_)...,
      std::forward<CaseFunc>(case_func)
    );
  } 
  void end() const {

  }
private:
  TestF test_f_;
  std::tuple<CaseItems...> case_items_;
};
template<class... Types>
struct Switcher {
  template<class TestF>
  auto operator()(TestF&& test_f) const {
    return SwitcherCase<TestF, Switcher>(std::forward<TestF>(test_f));
  }
};

}
template<class A>
struct TestFunc {
  template<class T>
  void is() const {
    decltype(a)
    return a.is<T>(); 
  }
  A a;
};
int main() {
  int a = 0;
  avalon::mpl::Switcher<int, double, float> switch_;
  switch_(TestFunc<decltype(a)>{a})
  .case_<int>([](){

  })
  .case_<double>([](){
    
  })
}