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
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <gmodule.h>
#include <gtkmm/enums.h>

#include "constants.h"
// clang-format on

namespace docklight
{
    namespace Config
    {
        GKeyFile* load_file();

        void AddArgs(const std::vector<std::tuple<gchar, int, Glib::ustring>>& args);
        void set_dock_location(dock_location_t location);
        void set_icon_size(int value);
        void set_separator_line(bool value);
        void set_dock_alignment(dock_alignment_t value);
        void set_default_style();

        int get_anchor_margin();
        int get_dock_area();
        int get_separator_margin();
        int get_window_start_end_margin();
        int get_icon_size();
        int get_separator_size();

        dock_indicator_type_t get_indicator_type();
        dock_alignment_t get_dock_alignment();

        bool is_autohide_none();
        bool is_autohide();
        bool is_intelihide();
        bool is_show_title();
        bool is_separator_line();

        float get_animation_delay();
        float get_hide_delay();
        //    Config* m_config = Config::getInstance();
    }  // namespace Config
}  // namespace docklight
