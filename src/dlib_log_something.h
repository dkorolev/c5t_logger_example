#pragma once

#include "dlib_generic.h"

namespace current::logger {
struct C5T_LOGGER_SINGLETON_Interface;
};

class IHasLoggerInterface : public virtual DLibGeneric {
 public:
  virtual current::logger::C5T_LOGGER_SINGLETON_Interface& Logger() const = 0;
};

extern "C" void LogSomethingFromDLib(DLibGeneric&);
