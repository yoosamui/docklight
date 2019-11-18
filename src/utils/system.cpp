#include "system.h"
#include <assert.h>
#include <dirent.h>
#include <limits.h>
#include <malloc.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <string>

#include <X11/Xlib.h>
#include <gdk/gdkx.h>
DL_NS_BEGIN

namespace system_util
{
    bool get_mouse_position(int& x, int& y)
    {
        Window window_returned;
        int root_x, root_y;
        int win_x, win_y;
        unsigned int mask_return;

        Display* display = XOpenDisplay(NULL);
        if (display == NULL) return false;

        Window root_window = XRootWindow(display, XDefaultScreen(display));
        Bool found = XQueryPointer(display, root_window, &window_returned, &window_returned,
                                   &root_x, &root_y, &win_x, &win_y, &mask_return);
        if (found) {
            x = win_x;
            y = win_y;
        }

        XCloseDisplay(display);
        return found;
    }

    string get_window_manager_name()
    {
        GdkScreen* screen = gdk_screen_get_default();
        const char* wm = gdk_x11_screen_get_window_manager_name(screen);

        return wm;
    }

    bool is_mutter_window_manager()
    {
        GdkScreen* screen = gdk_screen_get_default();
        const char* wm = gdk_x11_screen_get_window_manager_name(screen);
        if (wm && strncmp(wm, "Mutter", 5) == 0) {
            return true;
        }

        return false;
    }

    string get_current_path()
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

    int execute(const string& command_string)
    {
        char command[NAME_MAX];
        sprintf(command, "%s &", command_string.c_str());
        printf("COMMAND %s\n", command);
        return std::system(command);
    }

    const string get_current_path(const string& str)
    {
        string path = get_current_path();
        string result = path + "/" + str;
        return result;
    }

    bool file_exists(const std::string& name)
    {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

    // ignore case
    string file_exists(const string& directory, string& file_name)
    {
        DIR* dir = nullptr;
        dir = opendir(directory.c_str());
        if (dir == 0) {
            return {};
        }
        string result = {};

        struct dirent* hFile;
        while ((hFile = readdir(dir)) != nullptr) {
            if (!strcmp(hFile->d_name, ".")) continue;
            if (!strcmp(hFile->d_name, "..")) continue;

            if (strcasecmp(hFile->d_name, file_name.c_str()) == 0) {
                result = directory + hFile->d_name;
                break;
            }
        }

        closedir(dir);
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
        const int dir_err = mkdir(directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
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

    const string get_current_user()
    {
        uid_t uid = geteuid();
        struct passwd* pw = getpwuid(uid);
        if (pw) {
            return std::string(pw->pw_name);
        }
        return {};
    }
}  // namespace system_util

DL_NS_END

