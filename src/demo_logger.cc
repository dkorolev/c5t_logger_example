#include <thread>
#include <chrono>

#include "lib_c5t_logger.h"

#include "bricks/dflags/dflags.h"
#include "bricks/file/file.h"
#include "bricks/strings/join.h"
#include "bricks/strings/split.h"

int main(int argc, char** argv) {
  ParseDFlags(&argc, &argv);
  std::vector<std::string> argv0_path = current::strings::Split(current::Singleton<dflags::Argv0Container>().argv_0,
                                                                current::FileSystem::GetPathSeparator());
  argv0_path.pop_back();
  C5T_LOGGER_ACTIVATE(current::strings::Join(argv0_path, current::FileSystem::GetPathSeparator()));
  C5T_LOGGER("foo.log") << "this is foo, starting";
  for (size_t i = 0; i < 15; ++i) {
    C5T_LOGGER("chart.log") << i * i;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
  C5T_LOGGER("foo.log") << "this is foo, done";
  return 0;
}
