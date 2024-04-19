#pragma once

// Usage:
// 1) Create an interface provider as `class MyInterface : public virtual DLibGeneric`.
// 2) Have the `dlib`-exposed function be some `extern "C" void MyDLibExternalFunction(DLibGeneric& dlib);`
// 3) In that function, `dlib.Use<MyInterface>(...)`, see the `demo_*` in (some) C5T repo for details.
// 4) Call it with an instance of your `MyInterface`, see the `demo_*` in (some) C5T repo for details.

#include <functional>

struct DLibGeneric {
 protected:
  virtual ~DLibGeneric() = default;

 public:
  template <class I, class F, class G = std::function<decltype(std::declval<F>()(*std::declval<I*>()))()>>
  decltype(std::declval<F>()(*std::declval<I*>())) Use(
      F&& f, G&& g = []() -> decltype(std::declval<F>()(*std::declval<I*>())) {
        return decltype(std::declval<F>()(std::declval<I&>()))();
      }) {
    if (I* i = dynamic_cast<I*>(this)) {
      return f(*i);
    } else {
      return g();
    }
  }
};
