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
#include <iostream>

#include "components/config.h"
#include "utils/system.h"
// clang-format on

namespace docklight
{
    namespace config
    {
        // parsed and optimized at compile time
        constexpr const int DEF_MIN_ITEM_SIZE = 26;
        constexpr const int DEF_DOCKAREA_MARGIN = 4;
        constexpr const char* DEF_CONFIG_FILENAME = "docklight.config";
        constexpr const int DEF_ICON_SIZE = 48;
        constexpr const int DEF_ICON_MAXSIZE = 128;
        constexpr const int DEF_SEPARATOR_MARGIN = 12;
        constexpr const int DEF_SEPARATOR_SIZE = 8;
        constexpr const double DEF_AUTOHIDE_ANIMATION_DELAY = 5.0;
        constexpr const int DEF_AUTOHIDE_ANCHORT_MARGIN = 20;
        constexpr const double DEF_AUTOHIDE_HIDE_DELAY = 0.5;

        // Initializers
        int m_icon_size = DEF_ICON_SIZE;
        int m_separator_margin = DEF_SEPARATOR_MARGIN;
        int m_separator_size = DEF_SEPARATOR_SIZE;
        int m_anchor_margin = DEF_AUTOHIDE_ANCHORT_MARGIN;

        bool m_show_title = true;
        bool m_show_separator_line = true;

        float m_animation_delay = DEF_AUTOHIDE_ANIMATION_DELAY;
        float m_hide_delay = DEF_AUTOHIDE_HIDE_DELAY;

        dock_alignment_t m_alignment = dock_alignment_t::center;
        dock_autohide_type_t m_autohide_type = dock_autohide_type_t::intelihide;
        dock_location_t m_location = dock_location_t::bottom;
        dock_indicator_type_t m_indicator_type = dock_indicator_type_t::dots;

        std::string get_filepath()
        {
            std::string user_name = System::get_current_user();

            char config_dir[120];
            sprintf(config_dir, "/home/%s/.config/docklight", user_name.c_str());

            System::create_directory_if_not_exitst(config_dir);

            char buff[PATH_MAX];
            sprintf(buff, "%s/%s", config_dir, DEF_CONFIG_FILENAME);

            return buff;
        }

        GKeyFile* load_file()
        {
            set_default_style();

            std::string filepath = get_filepath();
            GError* error = nullptr;
            GKeyFile* key_file = g_key_file_new();

            auto found = g_key_file_load_from_file(key_file, filepath.c_str(),
                                                   GKeyFileFlags::G_KEY_FILE_NONE, &error);

            if (!found) {
                if (error) {
                    g_warning("Load: configuration could not be found. %s %s\n", error->message,
                              filepath.c_str());
                    g_error_free(error);
                    error = nullptr;
                }

                g_key_file_free(key_file);
                return nullptr;
            }

            return key_file;
        }

        void AddArgs(const std::vector<std::tuple<gchar, int, Glib::ustring>>& args)
        {
            for (auto&& t : args) {
                std::cout << "R:" << std::get<0>(t) << ", " << std::get<1>(t) << ", "
                          << std::get<2>(t) << std::endl;

                if (std::get<0>(t) == (char)'l') {
                    if (std::get<2>(t) == "bottom") {
                        m_location = dock_location_t::bottom;
                        continue;
                    }

                    if (std::get<2>(t) == "left") {
                        m_location = dock_location_t::left;
                        continue;
                    }

                    if (std::get<2>(t) == "top") {
                        m_location = dock_location_t::top;
                        continue;
                    }

                    if (std::get<2>(t) == "right") {
                        m_location = dock_location_t::right;
                        continue;
                    }
                }

                if (std::get<0>(t) == (char)'a') {
                    if (std::get<2>(t) == "start") {
                        m_alignment = dock_alignment_t::start;
                        continue;
                    }

                    if (std::get<2>(t) == "end") {
                        m_alignment = dock_alignment_t::end;
                        continue;
                    }

                    if (std::get<2>(t) == "center") {
                        m_alignment = dock_alignment_t::center;
                        continue;
                    }

                    if (std::get<2>(t) == "fill") {
                        m_alignment = dock_alignment_t::fill;
                        continue;
                    }
                }
            }
        }

        void set_default_style()
        {
            // clang-format off
        /*m_theme.set_Panel(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 1, 1, 0), 0, 3, 0));
        m_theme.set_PanelCell(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 1, 1, 1), 0.5, 3, 0));
        m_theme.set_PanelDrag(new ColorWindow(Color(1, 1, 1, 0.4), Color(1, 1, 1, 1), 2.5, 3, 0));
        m_theme.set_PanelIndicator(new ColorWindow(Color(1, 1, 1, 0.7), Color(1, 1, 1, 1), 2, 0, 0));
        m_theme.set_PanelSeparator(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 1, 1, 1.0), 1.0, 0, 0));
        m_theme.set_PanelTitle(new ColorWindow(Color(0, 0.50, 0.66, 1),Color(1, 1, 1, 1.0), 1.0, 0, 0));
        m_theme.set_Preview(new ColorWindow());
        m_theme.set_PreviewCell(new ColorWindow(Color(1, 1, 1, 0.2), Color(1, 1, 1, 1), 1, 3, 0));
        m_theme.set_PreviewTitleText(new ColorWindow(Color(1,1,1,0.4), Color(1, 1, 1, 1), 0, 0, 0));
        m_theme.set_PreviewClose(new ColorWindow(Color(0.854, 0.062, 0.133, 1), Color(1, 1, 1, 1), 2.0, 0, 0));*/
            // clang-format on
        }
        // get/set location
        dock_location_t get_dock_location()
        {
            //
            return m_location;
        }

        void set_dock_location(dock_location_t location)
        {
            //
            m_location = location;
        }

        int get_dock_area_margin()
        {
            return DEF_DOCKAREA_MARGIN;
        }
        int get_dock_area()
        {
            int area = m_icon_size + (DEF_DOCKAREA_MARGIN * 3) - (DEF_DOCKAREA_MARGIN / 2);

            return area;
        }

        Gtk::Orientation get_dock_orientation()
        {
            switch (m_location) {
                case dock_location_t::top:
                case dock_location_t::bottom:
                    return Gtk::ORIENTATION_HORIZONTAL;

                case dock_location_t::left:
                case dock_location_t::right:
                    return Gtk::ORIENTATION_VERTICAL;

                default:
                    return Gtk::ORIENTATION_HORIZONTAL;
            }
        }

        dock_alignment_t get_dock_alignment()
        {
            return m_alignment;
        }

        dock_indicator_type_t get_indicator_type()
        {
            return m_indicator_type;
        }

        void set_dock_alignment(dock_alignment_t value)
        {
            m_alignment = value;
        }

        int get_separator_margin()
        {
            //
            return m_separator_margin;
        }

        int get_window_start_end_margin()
        {
            if (get_dock_alignment() == dock_alignment_t::fill) {
                return 10;  // TODO: constant
            }

            // TODO: constant
            return 20;
        }

        int get_icon_size()
        {
            int icon_size = m_icon_size;
            return icon_size;
        }

        void set_icon_size(int value)
        {
            m_icon_size = value;

            if (value > DEF_ICON_MAXSIZE) {
                m_icon_size = DEF_ICON_MAXSIZE;
            }
        }

        bool is_autohide_none()
        {
            return m_autohide_type == dock_autohide_type_t::none;
        }

        bool is_autohide()
        {
            return m_autohide_type == dock_autohide_type_t::autohide;
        }

        bool is_intelihide()
        {
            return m_autohide_type == dock_autohide_type_t::intelihide;
        }

        bool is_separator_line()
        {
            return m_show_separator_line;
        }

        void set_separator_line(bool value)
        {
            m_show_separator_line = value;
        }

        float get_animation_delay()
        {
            //
            return m_animation_delay;
        }

        float get_hide_delay()
        {
            //
            return m_hide_delay;
        }

        int get_anchor_margin()
        {
            //
            return m_anchor_margin;
        }

    }  // namespace config
}  // namespace docklight
