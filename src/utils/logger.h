#pragma once

// Only private ofFile file;
//#include "ofFileUtils.h"
//#include <sstream>

/// \brief Convert a variable length argument to a string.
/// \param format A printf-style format string.
/// \param args A variable argument list.
/// \returns A string representation of the argument list.
///
template <typename... Args>
//__attribute__((__format__ (__printf__, 2, 0)))
std::string ofVAArgsToString(const char *format, Args &&...args)
{
    char buf[256];
    size_t n = std::snprintf(buf, sizeof(buf), format, std::forward<Args>(args)...);

    //	std::string str = format;
    //	size_t n = std::snprintf(buf, sizeof(buf), str, std::forward<Args>(args)...);

    // Static buffer large enough?
    if (n < sizeof(buf)) {
        return {buf, n};
    }

    // Static buffer too small
    std::string s(n + 1, 0);
    std::snprintf(const_cast<char *>(s.data()), s.size(), format, std::forward<Args>(args)...);

    return s;
}
