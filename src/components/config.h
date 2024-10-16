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
#include "components/configfile.h"
#include "utils/system.h"
#include "constants.h"
// clang-format on

namespace docklight
{
    class Configuration : public ConfigFile
    {
      public:
      public:
        Configuration();
        virtual ~Configuration();

        const Glib::ustring& get_monitor_name() const;

        const int get_dock_area_margin() const;
        const int get_anchor_margin() const;
        const int get_dock_area() const;
        //  const int get_separator_margin() const;
        void set_separator_size(guint size);
        const int get_icon_size() const;
        const int get_separator_size() const;

        const dock_icon_alignment_t get_dock_icon_alignment() const;
        const dock_alignment_t get_dock_alignment() const;
        const dock_indicator_type_t get_indicator_type() const;
        const dock_location_t get_dock_location() const;
        Glib::ustring get_dock_location_name() const;
        const Gtk::Orientation get_dock_orientation() const;

        const bool is_autohide_none() const;
        const bool is_autohide() const;
        const bool is_intelihide() const;

        const void set_icon_size(guint size);

        int get_custom_icon_size() const;
        int get_icon_max_size();
        const void set_custom_icon_size(const guint size);

        void set_arguments(const std::vector<std::tuple<gchar, int, Glib::ustring>>& args);
    };

    Glib::RefPtr<Configuration> Config();

}  // namespace docklight
