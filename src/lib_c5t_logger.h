#pragma once

#include <atomic>
#include <memory>
#include <fstream>
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

  struct InnerLoggerImpl final {
    // Keeps { log file name, fstream }, to re-create with one-liners.
    using active_t = std::pair<std::string, std::ofstream>;
    std::unique_ptr<active_t> active;
    ~InnerLoggerImpl();

    struct Construct final {};
    InnerLoggerImpl(Construct, std::string s) {}

    InnerLoggerImpl(InnerLoggerImpl const&) = delete;
    InnerLoggerImpl& operator=(InnerLoggerImpl const&) = delete;
    InnerLoggerImpl(InnerLoggerImpl&&) = delete;
    InnerLoggerImpl& operator=(InnerLoggerImpl&&) = delete;
  };

  current::WaitableAtomic<InnerLoggerImpl> inner_logger_;

  C5T_LOGGER_Impl() = delete;
  C5T_LOGGER_Impl(std::string log_file_name)
      : log_file_name_(std::move(log_file_name)), inner_logger_(InnerLoggerImpl::Construct(), log_file_name_) {}

  void WriteLine(std::string const&);

  template <typename T>
  C5T_LOGGER_LogLineWriter operator<<(T&& e) {
    C5T_LOGGER_LogLineWriter w(this);
    w << std::forward<T>(e);
    return w;
  }
};

struct C5T_LOGGER_SINGLETON_Impl final {
  struct InnerSingletonImpl final {
    std::atomic_bool initialized;
    std::string base_path;
    std::unordered_map<std::string, std::unique_ptr<C5T_LOGGER_Impl>> per_file_loggers;
    InnerSingletonImpl() : initialized(false) {}
  };
  current::WaitableAtomic<InnerSingletonImpl> inner_singleton_;
  std::atomic_bool const& initialized_;

  C5T_LOGGER_SINGLETON_Impl()
      : inner_singleton_(), initialized_(inner_singleton_.ImmutableScopedAccessor()->initialized) {}

  ~C5T_LOGGER_SINGLETON_Impl() {}

  C5T_LOGGER_SINGLETON_Impl& InitializedSelfOrAbort();

  void C5T_LOGGER_ACTIVATE_IMPL(std::string base_path);
  void C5T_LOGGER_LIST_Impl(std::function<void(std::string const& name, std::string const& latest_file)> cb) const;
  void C5T_LOGGER_FIND_Impl(std::string const& key,
                            std::function<void(std::string const& latest_file)> cb_found,
                            std::function<void()> notfound) const;

  C5T_LOGGER_Impl& operator[](std::string const& log_file_name);
};

struct C5T_LOGGER_SINGLETON_Holder final {
  C5T_LOGGER_SINGLETON_Impl* ptr = nullptr;
  C5T_LOGGER_SINGLETON_Impl& Use(C5T_LOGGER_SINGLETON_Impl& impl) {
    ptr = &impl;
    return impl;
  }
  C5T_LOGGER_SINGLETON_Impl& Val() {
    if (ptr == nullptr) {
      // TODO(dkorolev): Use some default one, right?
      ::abort();
    }
    return *ptr;
  }
};

#define C5T_LOGGER_CREATE_SINGLETON() \
  current::Singleton<C5T_LOGGER_SINGLETON_Holder>().Use(current::Singleton<C5T_LOGGER_SINGLETON_Impl>())
#define C5T_LOGGER_USE_PROVIDED(impl) current::Singleton<C5T_LOGGER_SINGLETON_Holder>().Use(impl)

#define C5T_LOGGER_ACTIVATE(...) C5T_LOGGER_CREATE_SINGLETON().C5T_LOGGER_ACTIVATE_IMPL(__VA_ARGS__)

#define C5T_LOGGER_RAW_INSTANCE() current::Singleton<C5T_LOGGER_SINGLETON_Holder>().Val()

#define C5T_LOGGER_INSTANCE() C5T_LOGGER_RAW_INSTANCE().InitializedSelfOrAbort()
#define C5T_LOGGER_LIST(cb) C5T_LOGGER_INSTANCE().C5T_LOGGER_LIST_Impl(cb)
#define C5T_LOGGER_FIND(key, cb_found, cb_notfound) \
  C5T_LOGGER_INSTANCE().C5T_LOGGER_FIND_Impl(key, cb_found, cb_notfound)

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
