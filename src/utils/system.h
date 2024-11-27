#pragma once
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

// clang-format off
#include <glibmm/main.h>
#include "constants.h"
// clang-format on

namespace docklight
{
    namespace system
    {
        bool get_mouse_position(int& x, int& y);
        bool is_mutter_window_manager();
        bool file_exists(std::string name);
        std::string file_exists(const std::string& directory, std::string& file_name);
        bool is_directory_exists(const char* directory_name);
        bool create_directory_if_not_exitst(const char* directory_name);

        Glib::ustring get_window_manager_name();
        Glib::ustring get_current_path(const Glib::ustring& str);
        Glib::ustring get_current_user();
        Glib::ustring get_current_path();

        /*
        bool file_exists(const Glib::ustring& name);
        bool create_directory_if_not_exitst(const char* directory_name);

        int execute(const std::string& command_string);

        Glib::ustring get_current_path(const Glib::ustring& str);

        const std::string get_current_user();
        std::string get_window_manager_name();
        std::string file_exists(const std::string& directory, std::string& file_name);*/

    }  // namespace system
}  // namespace docklight
