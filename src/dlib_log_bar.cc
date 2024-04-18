#include "dlib_log_something.h"
#include "lib_c5t_logger.h"

#include <iostream>

int cnt = 0;  // Will increment twice!

void LogSomethingFromDLib(C5T_LOGGER_SINGLETON_Interface& impl) {
  C5T_LOGGER_USE_PROVIDED(impl);
  std::cout << "bar starting\n";
  C5T_LOGGER("bar.log") << "this is bar, index = " << ++cnt;
  std::cout << "bar done\n";
}
