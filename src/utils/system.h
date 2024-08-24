#pragma once

#include <glibmm/main.h>

#include "constants.h"
namespace docklight
{

    namespace system_util
    {
        bool get_mouse_position(int& x, int& y);
        bool file_exists(const std::string& name);
        bool create_directory_if_not_exitst(const char* directory_name);
        bool is_mutter_window_manager();

        int execute(const std::string& command_string);

        Glib::ustring get_executable_path();
        const std::string get_current_path(const std::string& str);
        const std::string get_current_user();
        std::string get_window_manager_name();
        std::string file_exists(const std::string& directory, std::string& file_name);

    }  // namespace system_util
}  // namespace docklight
