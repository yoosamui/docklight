//  Copyright (c) 2018-2024 Juan R. González
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <X11/Xlib.h>
#include <assert.h>
#include <dirent.h>
#include <gdk/gdkx.h>
#include <limits.h>
#include <malloc.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <filesystem>
#include "system.h"
// clang-format on

namespace docklight
{
    namespace system
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

        Glib::ustring get_window_manager_name()
        {
            GdkScreen* screen = gdk_screen_get_default();
            const char* wm = gdk_x11_screen_get_window_manager_name(screen);

            return wm;
        }

        bool is_mutter_window_manager()
        {
            GdkScreen* screen = gdk_screen_get_default();
            const char* wm = gdk_x11_screen_get_window_manager_name(screen);
            if (wm && (strncmp(wm, "Mutter", 5) == 0 || strncmp(wm, "GNOME", 5) == 0 ||
                       strncmp(wm, "Muffin", 5) == 0)) {
                return true;
            }

            return false;
        }

        Glib::ustring get_current_user()
        {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);
            if (pw) {
                return std::string(pw->pw_name);
            }
            return {};
        }

        bool file_exists(std::string name)
        {
            struct stat buffer;
            return (stat(name.c_str(), &buffer) == 0);
        }
        // ignore case
        std::string file_exists(const std::string& directory, std::string& file_name)
        {
            DIR* dir = nullptr;
            dir = opendir(directory.c_str());
            if (dir == 0) {
                return {};
            }
            std::string result = {};

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

        Glib::ustring get_current_path()
        {
            char szTmp[32];
            sprintf(szTmp, "/proc/%d/exe", getpid());

            char buffer[PATH_MAX];
            ssize_t count = readlink(szTmp, buffer, PATH_MAX);
            Glib::ustring result = Glib::ustring(buffer, (count > 0) ? count : 0);
            size_t found = result.find_last_of("/");
            if (found != Glib::ustring::npos) result = result.substr(0, found);

            return result;
        }
    }  // namespace system
}  // namespace docklight
