#pragma once

#include <functional>

namespace current::logger {
struct C5T_LOGGER_SINGLETON_Interface;
};

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

class IHasLoggerInterface : public virtual DLibGeneric {
 public:
  virtual current::logger::C5T_LOGGER_SINGLETON_Interface& Logger() const = 0;
};

extern "C" void LogSomethingFromDLib(DLibGeneric&);
