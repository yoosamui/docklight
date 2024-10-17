
#pragma once
//  Copyright (c) 2018-2024 Juan R. González
//
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
//  public Glib::Object GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  identification number, along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <glibmm/object.h>
#include <giomm/desktopappinfo.h>
#include <gdk/gdk.h>

// clang-format on

namespace docklight
{

    void launcher(const Glib::ustring& desktop_file, const Glib::ustring& instance_name);
    void launcher2(const Glib::ustring& desktop_file, const Glib::ustring& instance_name,
                   const Glib::ustring& group_name, const Glib::ustring& icon_name);
}  // namespace docklight
