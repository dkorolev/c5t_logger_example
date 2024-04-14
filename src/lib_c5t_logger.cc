#include "lib_c5t_logger.h"

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

void C5T_LOGGER_SINGLETON_Impl::C5T_LOGGER_LIST_Impl(
    std::function<void(std::string const& name, std::string const& latest_file)> cb) const {
  inner_.ImmutableUse([&](Inner const& inner) {
    for (auto const& [k, v] : inner.per_file_loggers) {
      v->inner_.ImmutableUse([&](C5T_LOGGER_Impl::Inner const& inner2) {
        if (inner2.active) {
          cb(k, inner2.active->first);
        }
      });
    }
  });
}

void C5T_LOGGER_SINGLETON_Impl::C5T_LOGGER_FIND_Impl(std::string const& key,
                                                     std::function<void(std::string const& latest_file)> cb_found,
                                                     std::function<void()> cb_notfound) const {
  inner_.ImmutableUse([&](Inner const& inner) {
    auto const& e = inner.per_file_loggers.find(key);
    if (e != std::end(inner.per_file_loggers)) {
      e->second->inner_.ImmutableUse([&](C5T_LOGGER_Impl::Inner const& inner2) {
        if (inner2.active) {
          cb_found(inner2.active->first);
        } else {
          cb_notfound();
        }
      });
    } else {
      cb_notfound();
    }
  });
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

#if 1  // NOTE(dkorolev): I've tested this with Google Sheets by adding a chart. This date & time format works natively!
constexpr static char const* const kLogFmt = "%Y/%m/%d %H:%M:%S";  // The format that makes sense.
#else
constexpr static char const* const kLogFmt = "%m/%d/%Y %H:%M:%S";  // The US format.
#endif

#ifdef LOG_IMPL
#error "`LOG_IMPL` should not be `#define`-d by this point."
#endif
#define LOG_IMPL(s) A->second << TS.count() << '\t' << current::FormatDateTime(TS, kLogFmt) << '\t' << s << std::endl;

void C5T_LOGGER_Impl::WriteLine(std::string const& s) {
  auto const TS = current::time::Now();
  auto const ts = current::FormatDateTime(TS, "-%Y%m%d-%H");
  auto const fn = log_file_name_ + ts + ".txt";
  inner_.MutableUse([&](Inner& inner) {
    auto& A = inner.active;
    bool log_starting_new = false;
    bool create_or_recreate = false;
    if (!inner.active) {
      log_starting_new = true;
      create_or_recreate = true;
    } else if (inner.active->first != fn) {
      LOG_IMPL("moving on to " << fn);
      create_or_recreate = true;
    }
    if (create_or_recreate) {
      std::string renamed_file_name;
      try {
        if (current::FileSystem::GetFileSize(fn)) {
          renamed_file_name = fn + ts + '.' + current::ToString(TS.count()) + ".txt";
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
      LOG_IMPL("started a new log file")
    }
    LOG_IMPL(s);
  });
}

C5T_LOGGER_Impl::Inner::~Inner() {
  if (active && active->second) {
    auto const TS = current::time::Now();
    auto& A = active;
    LOG_IMPL("gracefully closing this log file");
  }
}

#undef LOG_IMPL
