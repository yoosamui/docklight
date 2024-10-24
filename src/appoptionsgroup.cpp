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
#include "appoptionsgroup.h"
#include "translations.h"
// clang-format on

namespace docklight
{

    AppOptionsGroup::AppOptionsGroup()
        : Glib::OptionGroup("Docklight",
                            "Supports short and long commandline options for the group.",
                            "help - Options can be single letters, prefixed by a single dash."),
          m_arg_monitor("primary"),
          m_arg_location("bottom"),
          m_arg_alignment("center")
    {
        m_entry_monitor.set_long_name("monitor");
        m_entry_monitor.set_short_name('m');
        m_entry_monitor.set_description(MSG_SETS_THE_PRIMARY_MONITOR);
        add_entry(m_entry_monitor, m_arg_monitor);

        m_entry_location.set_long_name("location");
        m_entry_location.set_short_name('l');
        m_entry_location.set_description(MSG_SETS_THE_PANEL_LOCATION);
        add_entry(m_entry_location, m_arg_location);

        m_entry_alignment.set_long_name("alignment");
        m_entry_alignment.set_short_name('a');
        m_entry_alignment.set_description(MSG_SETS_THE_DOCK_ALIGNMENT);
        add_entry(m_entry_alignment, m_arg_alignment);

        m_entry_icon_alignment.set_long_name("icon_alignment");
        m_entry_icon_alignment.set_short_name('i');
        m_entry_icon_alignment.set_description(MSG_SETS_THE_ICON_ALIGNMENT);
        add_entry(m_entry_icon_alignment, m_arg_icon_alignment);
    }

    AppOptionsGroup::~AppOptionsGroup()
    {
        g_message(MSG_FREE_OBJECT, "AppOptionsGroup");
    }

    void AppOptionsGroup::validate()
    {
        validate_location();
        validate_monitor();
        validate_alignment();
        validate_icon_alignment();
    }

    Glib::ustring AppOptionsGroup::toString() const
    {
        std::stringstream result;
        result << MSG_PARSED_VALUES << " :" << std::endl
               << "  monitor"
               << " :" << m_arg_monitor << std::endl
               << "  location"
               << " :" << m_arg_location << std::endl
               << "  alignment"
               << " :" << m_arg_alignment << std::endl;

        return result.str();
    }

    void AppOptionsGroup::validate_monitor()
    {
        if (m_arg_monitor == "primary") return;

        Glib::ustring monitor_list = "";

        for (int i = 0; i < device::monitor::get_monitor_count(); i++) {
            auto const m = device::monitor::get_monitor(i);

            if (m_arg_monitor == m->get_model()) return;
            monitor_list += (Glib::ustring)m->get_model() + ", ";
        }

        std::stringstream msg;
        msg << "m_arg_monitor: unexpected value of monitor: " << m_arg_monitor << std::endl
            << m_entry_monitor.get_description() << std::endl
            << "Valid arguments are: primary, " + monitor_list << std::endl
            << std::endl;

        throw Glib::OptionError(Glib::OptionError::BAD_VALUE, msg.str());
    }

    void AppOptionsGroup::validate_location()
    {
        if (m_arg_location == "top" || m_arg_location == "right" || m_arg_location == "bottom" ||
            m_arg_location == "left") {
            return;
        }

        std::stringstream msg;
        msg << "m_arg_location: unexpected option name: " << m_arg_location << std::endl
            << m_entry_location.get_description() << std::endl;

        throw Glib::OptionError(Glib::OptionError::UNKNOWN_OPTION, msg.str());
    }

    void AppOptionsGroup::validate_alignment()
    {
        if (m_arg_alignment == "start" || m_arg_alignment == "end" || m_arg_alignment == "center" ||
            m_arg_alignment == "fill") {
            return;
        }

        std::stringstream msg;
        msg << "m_arg_alignment: unexpected option name: " << m_arg_alignment << "\n"
            << m_entry_alignment.get_description() << std::endl;

        throw Glib::OptionError(Glib::OptionError::UNKNOWN_OPTION, msg.str());
    }

    void AppOptionsGroup::validate_icon_alignment()
    {
        if (m_arg_alignment == "start" || m_arg_alignment == "end" || m_arg_alignment == "center" ||
            m_arg_alignment == "fill") {
            return;
        }

        std::stringstream msg;
        msg << "m_arg_icon_alignment: unexpected option name: " << m_arg_icon_alignment << "\n"
            << m_entry_icon_alignment.get_description() << std::endl;

        throw Glib::OptionError(Glib::OptionError::UNKNOWN_OPTION, msg.str());
    }

    const std::vector<std::tuple<gchar, int, Glib::ustring>>& AppOptionsGroup::getList()
    {
        m_list.push_back(std::make_tuple(m_entry_monitor.get_short_name(), 0, m_arg_monitor));
        m_list.push_back(std::make_tuple(m_entry_location.get_short_name(), 0, m_arg_location));
        m_list.push_back(std::make_tuple(m_entry_alignment.get_short_name(), 0, m_arg_alignment));
        m_list.push_back(
            std::make_tuple(m_entry_icon_alignment.get_short_name(), 0, m_arg_icon_alignment));

        return m_list;
    }
}  // namespace docklight
