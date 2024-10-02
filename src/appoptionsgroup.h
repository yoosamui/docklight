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

// https://gitlab.gnome.org/GNOME/glibmm/-/blob/master/examples/options/main.cc

// clang-format off
#include <glibmm.h>
#include <iomanip>
#include <iostream>

#include "components/device.h"

// clang-format on

// Commandline Option Parser
// https://gitlab.gnome.org/GNOME/glibmm/-/blob/master/examples/options/main.cc
//
// The OptionGroup commandline parser is intended to be a simpler replacement
// for the popt library. It supports short and long commandline options, as shown
// in the following example:
//
// main -r 1 --foo=34 --max-size 20 --rand --display=:1.0 -vb -- file1 file2

namespace docklight
{
    class AppOptionsResult
    {
      public:
        gchar short_name;
        Glib::ustring long_name;
    };  // namespace AppOptionsResult

    class AppOptionsGroup : public Glib::OptionGroup
    {
      public:
        AppOptionsGroup();
        ~AppOptionsGroup();

        const std::vector<std::tuple<gchar, int, Glib::ustring>>& getList();
        void validate();
        Glib::ustring toString() const;

      private:
        void validate_location();
        void validate_monitor();
        void validate_alignment();
        void validate_icon_alignment();

      private:
        std::vector<std::tuple<gchar, int, Glib::ustring>> m_list;

        Glib::OptionEntry m_entry_monitor;
        Glib::OptionEntry m_entry_location;
        Glib::OptionEntry m_entry_alignment;
        Glib::OptionEntry m_entry_icon_alignment;

        int m_arg_monitor;

        Glib::ustring m_arg_location;
        Glib::ustring m_arg_alignment;
        Glib::ustring m_arg_icon_alignment;
    };
}  // namespace docklight

