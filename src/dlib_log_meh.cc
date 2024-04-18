#include "dlib_log_something.h"
#include "lib_c5t_logger.h"

// TODO: Move `IHasLoggerInterface` into the logger header!
void LogSomethingFromDLib(IUnknown const* interfaces) {
  if (auto ilogger = dynamic_cast<IHasLoggerInterface const*>(interfaces)) {
    C5T_LOGGER_USE(ilogger->Logger());
    C5T_LOGGER("meh.log") << "meh1";
    C5T_LOGGER("meh.log") << "meh2";
    C5T_LOGGER("meh.log") << "meh3";
    C5T_LOGGER("moremeh.log") << "more meh!";
  } else {
    std::cout << "meh can't work, no logger." << std::endl;
  }
}
