#pragma once

namespace current::logger {
struct C5T_LOGGER_SINGLETON_Interface;
};

// TODO: Move IUnknown somewhere up.
struct IUnknown {
 protected:
  virtual ~IUnknown() = default;
};

class IHasLoggerInterface : public virtual IUnknown {
 protected:
  ~IHasLoggerInterface() = default;

 public:
  virtual current::logger::C5T_LOGGER_SINGLETON_Interface& Logger() const = 0;
};

extern "C" void LogSomethingFromDLib(IUnknown const*);
