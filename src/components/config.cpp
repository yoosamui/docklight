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
// clang-format on

namespace docklight
{
    Glib::RefPtr<Configuration> m_config;
    Glib::RefPtr<Configuration> create_config()
    {
        if (!m_config) {
            m_config = Glib::RefPtr<Configuration>(new Configuration());
        }

        return m_config;
    }

    Glib::RefPtr<Configuration> Config()
    {
        if (!m_config) {
            m_config = create_config();
        }

        return m_config;
    }

    Configuration::Configuration()
    {
        g_message("Create Configuration.");
    }

    Configuration::~Configuration()
    {
        //
    }

    Theme& Configuration::get_theme()
    {
        return m_theme;
    }

    const Glib::ustring& Configuration::get_monitor_name() const
    {
        return m_monitor_name;
    }

    int Configuration::get_dock_area_margin() const
    {
        return DEF_DOCKAREA_MARGIN;
    }

    int Configuration::get_anchor_margin() const
    {
        //
        return m_anchor_margin;
    }

    int Configuration::get_dock_area() const
    {
        return m_icon_size + DEF_DOCKAREA_MARGIN;
    }

    // const int Configuration::get_separator_margin() const
    //{
    ////
    // return m_separator_margin;
    //}n

    int Configuration::get_icon_size() const
    {
        return m_icon_size;
    }

    int Configuration::get_custom_icon_size() const
    {
        return m_custom_icon_size;
    }

    int Configuration::get_custom_image_size() const
    {
        return m_custom_image_size;
    }

    int Configuration::get_icon_max_size()
    {
        return DEF_ICON_MAXSIZE;
    }

    int Configuration::get_preview_image_max_size()
    {
        return DEF_PREVIEW_IMAGE_MAX_SIZE;
    }

    int Configuration::get_preview_image_size() const
    {
        return m_image_size;
    }

    int Configuration::get_preview_area_margin() const
    {
        return DEF_PREVIEWAREA_MARGIN;
    }

    int Configuration::get_preview_area() const
    {
        auto value = m_image_size + get_preview_area_margin();
        return value;
    }

    void Configuration::set_custom_icon_size(guint size)
    {
        if (!size || size > (guint)DEF_ICON_MAXSIZE) return;

        m_custom_icon_size = size;
        if (size == 0) m_custom_icon_size = m_icon_size;
    }

    void Configuration::set_icon_size(guint size)
    {
        if (!size || size > (guint)DEF_ICON_MAXSIZE) return;
        m_icon_size = size;
    }

    void Configuration::set_image_size(guint size)
    {
        if (!size || size > (guint)DEF_PREVIEW_IMAGE_MAX_SIZE) return;
        m_image_size = size;
    }

    void Configuration::set_separator_size(guint size)
    {
        if (!size || size > 64) return;
        m_separator_size = size;
    }

    int Configuration::get_separator_size() const
    {
        return m_separator_size;
    }

    dock_icon_alignment_t Configuration::get_dock_icon_alignment() const
    {
        return m_icon_alignment;
    }

    dock_alignment_t Configuration::get_dock_alignment() const
    {
        return m_alignment;
    }

    dock_indicator_type_t Configuration::get_indicator_type() const
    {
        return m_indicator_type;
    }

    dock_location_t Configuration::get_dock_location() const
    {
        //
        return m_location;
    }

    Glib::ustring Configuration::get_dock_location_name() const
    {
        Glib::ustring location_name;

        switch (m_location) {
            case dock_location_t::top:
                location_name = "top";
                break;
            case dock_location_t::bottom:
                location_name = "bottom";
                break;

            case dock_location_t::left:
                location_name = "left";
                break;
            case dock_location_t::right:
                location_name = "right";
                break;
        }

        return location_name;
    }

    Gtk::Orientation Configuration::get_dock_orientation() const
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

    bool Configuration::is_autohide_none() const
    {
        return m_autohide_mode == dock_autohide_type_t::none;
    }

    bool Configuration::is_autohide() const
    {
        return m_autohide_mode == dock_autohide_type_t::autohide;
    }

    bool Configuration::is_intelihide() const
    {
        return m_autohide_mode == dock_autohide_type_t::intelihide;
    }

    void Configuration::set_arguments(
        const std::vector<std::tuple<gchar, int, Glib::ustring>>& args)
    {
        if (!load()) return;
        // ARGS
        for (auto&& t : args) {
            if (std::get<0>(t) == (char)'m') {
                m_monitor_name = std::get<2>(t);
                continue;
            }

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

            if (std::get<0>(t) == (char)'i') {
                if (std::get<2>(t) == "start") {
                    m_icon_alignment = dock_icon_alignment_t::start;
                    continue;
                }

                if (std::get<2>(t) == "end") {
                    m_icon_alignment = dock_icon_alignment_t::end;
                    continue;
                }

                if (std::get<2>(t) == "center") {
                    m_icon_alignment = dock_icon_alignment_t::center;
                    continue;
                }
            }
        }
    }
}  // namespace docklight
