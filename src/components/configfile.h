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
//
//#include <glibmm.h>
#include <glibmm/main.h>
#include <glibmm/object.h>
#include <gtkmm/enums.h>

#include "utils/system.h"
#include "constants.h"
// clang-format on

namespace docklight
{

    class ConfigBase : public Glib::Object
    {
      public:
        static constexpr const int DEF_INDICATOR_SIZE = 2;
        static const int DEF_ICON_MAXSIZE = 128;

      protected:
        // parsed and optimized at compile time
        static constexpr const int DEF_MIN_ITEM_SIZE = 24;
        // no access from draussen
        static constexpr const int DEF_DOCKAREA_MARGIN = 12;
        static constexpr const int DEF_ICON_SIZE = 128;
        // static constexpr const int DEF_SEPARATOR_MARGIN = 0;  // OBSOLETE
        static constexpr const int DEF_SEPARATOR_SIZE = 0;
        static constexpr const double DEF_AUTOHIDE_ANIMATION_DELAY = 5.0;
        static constexpr const int DEF_AUTOHIDE_ANCHORT_MARGIN = 20;
        static constexpr const double DEF_AUTOHIDE_HIDE_DELAY = 0.5;

        // Initializers
        int m_icon_size = DEF_ICON_SIZE;
        int m_custom_icon_size = DEF_ICON_SIZE;

        int m_separator_size = DEF_SEPARATOR_SIZE;
        int m_separator_size_factor = DEF_SEPARATOR_SIZE;

        // int m_separator_margin = DEF_SEPARATOR_MARGIN;

        int m_anchor_margin = DEF_AUTOHIDE_ANCHORT_MARGIN;

        bool m_show_title = true;
        bool m_show_separator_line = true;

        float m_animation_delay = DEF_AUTOHIDE_ANIMATION_DELAY;
        float m_hide_delay = DEF_AUTOHIDE_HIDE_DELAY;

        dock_alignment_t m_alignment = dock_alignment_t::center;
        dock_icon_alignment_t m_icon_alignment = dock_icon_alignment_t::center;

        //        TODO : TEST
        // dock_autohide_type_t m_autohide_type = dock_autohide_type_t::intelihide;
        dock_autohide_type_t m_autohide_type = dock_autohide_type_t::none;

        dock_location_t m_location = dock_location_t::bottom;
        dock_indicator_type_t m_indicator_type = dock_indicator_type_t::dots;

        Glib::ustring m_monitor_name = "primary";
    };

    class ConfigFile : public ConfigBase
    {
      protected:
      public:
        ConfigFile();
        ~ConfigFile();

        bool load();

      protected:
        std::string read_filepath();
        std::string read_location();
        std::string read_alignment();
        std::string read_indicator_type_key();
        std::string read_monitor_name();

        bool read_separator_show_line();

        int read_icon_size();
        int read_separator_size();
        int read_separator_margin();
        int read_anchor_margin();

        float read_animation_delay();
        float read_hide_delay();

      private:
        std::string m_filename = "docklight5.config";
        GKeyFile* m_key_file = nullptr;
    };

}  // namespace docklight
