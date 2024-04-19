#include "dlib_log_something.h"
#include "lib_c5t_logger.h"

#include <iostream>

int cnt = 0;  // Will increment twice!

void LogSomethingFromDLib(DLibGeneric& interfaces) {
  if (!interfaces.Use<IHasLoggerInterface>(
          [](IHasLoggerInterface& yes) {
            C5T_LOGGER_USE(yes.Logger());
            std::cout << "bar starting\n";
            C5T_LOGGER("bar.log") << "this is bar, index = " << ++cnt;
            std::cout << "bar done\n";
            return true;
          },
          []() { return false; })) {
    std::cout << "no IHasLoggerInterface" << std::endl;
  }
}
