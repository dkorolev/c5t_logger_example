#pragma once

namespace current::logger {
struct C5T_LOGGER_SINGLETON_Interface;
};

class IHasLoggerInterface {
 protected:
  ~IHasLoggerInterface() = default;

 public:
  virtual current::logger::C5T_LOGGER_SINGLETON_Interface& Logger() const = 0;
};

extern "C" void LogSomethingFromDLib(IHasLoggerInterface const*);
