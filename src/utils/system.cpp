
#include "system.h"
#include <assert.h>
#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <string>

DL_NS_BEGIN

namespace system_util
{
    inline string get_current_path()
    {
        char szTmp[32];
        sprintf(szTmp, "/proc/%d/exe", getpid());

        char buffer[PATH_MAX];
        ssize_t count = readlink(szTmp, buffer, PATH_MAX);
        string result = string(buffer, (count > 0) ? count : 0);
        size_t found = result.find_last_of("/");
        if (found != string::npos) result = result.substr(0, found);

        return result;
    }

    const string get_current_path(const string& str)
    {
        string path = get_current_path();
        string result = path + "/" + str;
        return result;
    }
}  // namespace system_util
DL_NS_END
