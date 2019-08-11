
#include "system.h"
#include <assert.h>
#include <dirent.h>
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

    bool is_directory_exists(const char* directory_name)
    {
        if (directory_name == nullptr) return false;

        DIR* dir;
        bool exists = false;

        dir = opendir(directory_name);

        if (dir != nullptr) {
            exists = true;
            (void)closedir(dir);
        }

        return exists;
    }

    bool create_directory(const char* directory_name)
    {
        const int dir_err =
            mkdir(directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (dir_err == -1) {
            return false;
        }

        return true;
    }

    bool create_directory_if_not_exitst(const char* directory_name)
    {
        if (!is_directory_exists(directory_name)) {
            return create_directory(directory_name);
        }

        return false;
    }
}  // namespace system_util

DL_NS_END

