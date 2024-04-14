#include "lib_c5t_logger.h"

// #include <chrono>
#include <fstream>

#include "bricks/exception.h"
#include "bricks/time/chrono.h"
#include "bricks/file/file.h"

// TODO(dkorolev): Mark idle-ness?

C5T_LOGGER_LogLineWriter::~C5T_LOGGER_LogLineWriter() {
  std::string s = oss_.str();
  if (to_file_) {
    self_->WriteLine(s);
  }
}

C5T_LOGGER_SINGLETON_Impl& C5T_LOGGER_SINGLETON_Impl::InitializedSelfOrAbort() {
  if (!initialized_) {
    ::perror("Was not `C5T_LOGGER_ACTIVATE()`-d, aborting.");
    ::abort();
  }
  return *this;
}
void C5T_LOGGER_SINGLETON_Impl::C5T_LOGGER_ACTIVATE_IMPL(std::string base_path) {
  if (!inner_.MutableUse([&](Inner& inner) {
        if (inner.initialized) {
          return false;
        }
        inner.initialized = true;
        inner.base_path = std::move(base_path);
        return true;
      })) {
    ::perror("Was not `C5T_LOGGER_ACTIVATE()`-d, aborting.");
    ::abort();
  }
}

C5T_LOGGER_Impl& C5T_LOGGER_SINGLETON_Impl::operator[](std::string const& log_file_name) {
  return InitializedSelfOrAbort().inner_.MutableUse([&](Inner& inner) -> C5T_LOGGER_Impl& {
    auto& placeholder = inner.per_file_loggers[log_file_name];
    if (!placeholder) {
      placeholder = std::make_unique<C5T_LOGGER_Impl>(current::FileSystem::JoinPath(inner.base_path, log_file_name));
    }
    return *placeholder;
  });
}

C5T_LOGGER_Impl& C5T_LOGGER(std::string const& name) { return C5T_LOGGER_SINGLETON_IMPL()[name]; }

void C5T_LOGGER_Impl::WriteLine(std::string const& s) {
  auto const now = current::time::Now();
  auto const fn = log_file_name_ + current::FormatDateTime(now, "-%Y%m%d-%H");
  inner_.MutableUse([&](Inner& inner) {
#ifdef LOG_IMPL
#error "`LOG_IMPL` should not be `#define`-d by this point."
#endif
#define LOG_IMPL(s)                                                                                             \
  inner.active->second << now.count() << '\t' << current::FormatDateTime(now, "%Y/%m/%d-%H:%M:%S") << '\t' << s \
                       << std::endl;
    bool log_starting_new = false;
    bool log_moving_on = false;
    bool create_or_recreate = false;
    if (!inner.active) {
      log_starting_new = true;
      create_or_recreate = true;
    } else if (inner.active->first != fn) {
      log_moving_on = true;
      create_or_recreate = true;
    }
    if (log_moving_on) {
      LOG_IMPL("moving on to " << fn);
    }
    if (create_or_recreate) {
      std::string renamed_file_name;
      try {
        if (current::FileSystem::GetFileSize(fn)) {
          renamed_file_name = fn + "." + current::ToString(now.count());
          current::FileSystem::RenameFile(fn, renamed_file_name);
        }
      } catch (current::Exception const&) {
        // Consciously ignore exceptions here.
      }
      inner.active = std::make_unique<Inner::active_t>(fn, std::ofstream(fn, std::fstream::app | std::fstream::ate));
      if (!renamed_file_name.empty()) {
        LOG_IMPL("the old log file was renamed to " << renamed_file_name);
      }
    }
    if (log_starting_new) {
      LOG_IMPL("started the new log file")
    }
    inner.active->second << now.count() << '\t' << current::FormatDateTime(now, "%Y/%m/%d-%H:%M:%S") << '\t' << s
                         << std::endl;
#undef LOG_IMPL
  });
}
