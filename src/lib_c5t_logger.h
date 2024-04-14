#pragma once

#include <atomic>
#include <memory>
#include <sstream>

#include "bricks/sync/waitable_atomic.h"
#include "bricks/util/singleton.h"  // IWYU pragma: keep

struct C5T_LOGGER_Impl;

struct C5T_LOGGER_LogLineWriter final {
  C5T_LOGGER_Impl* self_;
  bool to_file_;
  std::ostringstream oss_;

  C5T_LOGGER_LogLineWriter() = delete;
  C5T_LOGGER_LogLineWriter(C5T_LOGGER_LogLineWriter&&) = default;

  explicit C5T_LOGGER_LogLineWriter(C5T_LOGGER_Impl* self, bool to_file = true) : self_(self), to_file_(to_file) {}

  template <typename T>
  C5T_LOGGER_LogLineWriter& operator<<(T&& e) {
    oss_ << std::forward<T>(e);
    return *this;
  }

  ~C5T_LOGGER_LogLineWriter();
};

struct C5T_LOGGER_Impl final {
  std::string const log_file_name_;

  struct Inner final {
    // Keeps { log file name, fstream }, to re-create with one-liners.
    using active_t = std::pair<std::string, std::ofstream>;
    std::unique_ptr<active_t> active;
    ~Inner();
  };

  current::WaitableAtomic<Inner> inner_;

  C5T_LOGGER_Impl() = delete;
  C5T_LOGGER_Impl(std::string log_file_name) : log_file_name_(std::move(log_file_name)) {}

  void WriteLine(std::string const&);

  template <typename T>
  C5T_LOGGER_LogLineWriter operator<<(T&& e) {
    C5T_LOGGER_LogLineWriter w(this);
    w << std::forward<T>(e);
    return w;
  }
};

struct C5T_LOGGER_SINGLETON_Impl final {
  struct Inner final {
    std::atomic_bool initialized;
    std::string base_path;
    std::unordered_map<std::string, std::unique_ptr<C5T_LOGGER_Impl>> per_file_loggers;
    Inner() : initialized(false) {}
  };
  current::WaitableAtomic<Inner> inner_;
  std::atomic_bool const& initialized_;

  C5T_LOGGER_SINGLETON_Impl() : inner_(), initialized_(inner_.ImmutableScopedAccessor()->initialized) {}

  C5T_LOGGER_SINGLETON_Impl& InitializedSelfOrAbort();

  void C5T_LOGGER_ACTIVATE_IMPL(std::string base_path);
  void C5T_LOGGER_LIST_Impl(std::function<void(std::string const& name, std::string const& latest_file)> cb) const;

  C5T_LOGGER_Impl& operator[](std::string const& log_file_name);
};

#define C5T_LOGGER_SINGLETON_RAW_IMPL() current::Singleton<C5T_LOGGER_SINGLETON_Impl>()
#define C5T_LOGGER_SINGLETON_IMPL() current::Singleton<C5T_LOGGER_SINGLETON_Impl>().InitializedSelfOrAbort()
#define C5T_LOGGER_ACTIVATE(...) C5T_LOGGER_SINGLETON_RAW_IMPL().C5T_LOGGER_ACTIVATE_IMPL(__VA_ARGS__)
#define C5T_LOGGER_LIST(cb) C5T_LOGGER_SINGLETON_IMPL().C5T_LOGGER_LIST_Impl(cb)

C5T_LOGGER_Impl& C5T_LOGGER(std::string const& name);

#if 0

// NOTE(dkorolev): Non-persisted logs are an idea for the future when there are HTTP-based ways to listen to logs.

struct C5T_LOGGER_Impl_NoPersist final {
  C5T_LOGGER_Impl* impl_;
  C5T_LOGGER_Impl_NoPersist() = delete;
  explicit C5T_LOGGER_Impl_NoPersist(C5T_LOGGER_Impl& impl) : impl_(&impl) {}

  template <typename T>
  C5T_LOGGER_LogLineWriter operator<<(T&& e) {
    C5T_LOGGER_LogLineWriter w(impl_, false);
    w << std::forward<T>(e);
    return w;
  }
};

inline C5T_LOGGER_Impl_NoPersist C5T_LOGGER_NOPERSIST(std::string const& name) {
  return C5T_LOGGER_Impl_NoPersist(C5T_LOGGER(name));
}

#endif
