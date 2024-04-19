#include "dlib_log_something.h"
#include "lib_c5t_logger.h"

// TODO: Move `IHasLoggerInterface` into the logger header!
void LogSomethingFromDLib(DLibGeneric& interfaces) {
  interfaces.Use<IHasLoggerInterface>(
      [](IHasLoggerInterface& logger) {
        C5T_LOGGER_USE(logger.Logger());
        C5T_LOGGER("meh.log") << "meh1";
        C5T_LOGGER("meh.log") << "meh2";
        C5T_LOGGER("meh.log") << "meh3";
        C5T_LOGGER("moremeh.log") << "more meh!";
      },
      []() {
        std::cout << "meh can't work, no logger." << std::endl;
      });
}
