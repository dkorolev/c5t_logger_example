#pragma once

namespace current::logger {
struct C5T_LOGGER_SINGLETON_Interface;
};

extern "C" void LogSomethingFromDLib(current::logger::C5T_LOGGER_SINGLETON_Interface&);
