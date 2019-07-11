#include "utils/string.h"

DL_NS_BEGIN

namespace string_util {
/**
 * Validate numeric strings.
 * returns true id the string is numeric, otherwise false.
 */
bool is_numeric(const char *arg) {
  string s(arg);
  return is_numeric(s);
}

/**
 * Validate numeric strings.
 * returns true id the string is numeric, otherwise false.
 */
bool is_numeric(const string &s) {
  std::string::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it))
    ++it;
  return !s.empty() && it == s.end();
}
} // namespace string_util
DL_NS_END
