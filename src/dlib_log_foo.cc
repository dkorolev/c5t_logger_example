#include "dlib_log_something.h"

#include <iostream>

void LogSomethingFromDLib(IHasLoggerInterface const*) { std::cout << "foo!\n"; }
