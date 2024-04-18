#include "dlib_log_something.h"
#include "lib_c5t_logger.h"

#include <iostream>

int cnt = 0;  // Will increment twice!

// TODO: Move `IHasLoggerInterface` into the logger header!
void LogSomethingFromDLib(IUnknown const* interfaces) {
  if (auto ilogger = dynamic_cast<IHasLoggerInterface const*>(interfaces)) {
    C5T_LOGGER_USE(ilogger->Logger());
    std::cout << "bar starting\n";
    C5T_LOGGER("bar.log") << "this is bar, index = " << ++cnt;
    std::cout << "bar done\n";
  } else {
    std::cout << "no IHasLoggerInterface" << std::endl;
  }
}
