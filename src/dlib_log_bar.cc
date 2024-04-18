#include "dlib_log_something.h"
#include "lib_c5t_logger.h"

#include <iostream>

int cnt = 0;  // Will increment twice!

void LogSomethingFromDLib(current::logger::C5T_LOGGER_SINGLETON_Interface& c5t_logger) {
  C5T_LOGGER_USE(c5t_logger);
  std::cout << "bar starting\n";
  C5T_LOGGER("bar.log") << "this is bar, index = " << ++cnt;
  std::cout << "bar done\n";
}
