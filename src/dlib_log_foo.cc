#include "dlib_log_something.h"

#include <iostream>

void LogSomethingFromDLib(current::logger::C5T_LOGGER_SINGLETON_Interface&) { std::cout << "foo!\n"; }
