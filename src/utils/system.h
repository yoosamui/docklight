#pragma once

#include "common.h"

DL_NS_BEGIN

namespace system_util
{
    bool get_mouse_position(int& x, int& y);
    int execute(const string& command_string);
    bool file_exists(const std::string& name);
    string file_exists(const string& directory, string& file_name);
    const string get_current_path(const string& str);
    bool create_directory_if_not_exitst(const char* directory_name);
    const string get_current_user();
    bool is_mutter_window_manager();
    string get_window_manager_name();

}  // namespace system_util

DL_NS_END

