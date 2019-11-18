
#include "utils/file.h"
#include <sys/stat.h>

DL_NS_BEGIN

namespace file_util {
bool is_file_exits(const string &file_path) {
  struct stat buffer;
  return (stat(file_path.c_str(), &buffer) == 0);
}

} // namespace file_util
DL_NS_END
