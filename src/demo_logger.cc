#include <thread>
#include <chrono>

#include "lib_c5t_logger.h"

#include "bricks/dflags/dflags.h"
#include "bricks/file/file.h"
#include "bricks/strings/join.h"
#include "bricks/strings/split.h"
#include "bricks/system/syscalls.h"

int main(int argc, char** argv) {
  ParseDFlags(&argc, &argv);
  std::string const bin_path = []() {
    std::string const argv0 = current::Singleton<dflags::Argv0Container>().argv_0;
    std::vector<std::string> argv0_path = current::strings::Split(argv0, current::FileSystem::GetPathSeparator());
    argv0_path.pop_back();
    std::string const res = current::strings::Join(argv0_path, current::FileSystem::GetPathSeparator());
    return argv0[0] == current::FileSystem::GetPathSeparator() ? "/" + res : res;
  }();

  C5T_LOGGER_ACTIVATE(bin_path);

  C5T_LOGGER("demo.log") << "this is the demo, starting";
  C5T_LOGGER_LIST([](std::string const& name, std::string const& filename) {
    std::cerr << name << " => " << filename << std::endl;
  });

  for (std::string s : {"foo", "bar", "foo", "bar"}) {
    auto dl = current::bricks::system::DynamicLibrary::CrossPlatform(bin_path + "/libdlib_log_" + s);
    auto pf = dl.template Get<void (*)(current::logger::C5T_LOGGER_SINGLETON_Interface&)>("LogSomethingFromDLib");
    if (pf) {
      (*pf)(current::logger::C5T_LOGGER_INSTANCE());
    }
  }

  // This test that the output date format is friendly with Google Spreadsheets =)
  constexpr size_t N = 15;
  for (size_t i = 0; i < 15; ++i) {
    C5T_LOGGER("chart.log") << i * i;
    std::cout << i << " / " << N << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  C5T_LOGGER("foo.log") << "this is foo, done";
  return 0;
}
