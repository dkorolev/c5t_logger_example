#include "dlib_log_something.h"

#include <iostream>

void LogSomethingFromDLib(C5T_LOGGER_SINGLETON_Impl&) {
  std::cout << "foo!\n";
}
