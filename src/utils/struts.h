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
#include <gtkmm/window.h>
#include "components/device.h"
#include "components/config.h"
// clang-format on

namespace docklight::position
{

    class Struts : public Glib::Object
    {
        typedef enum struts_enum {
            left = 0,
            right = 1,
            top = 2,
            bottom = 3,
            left_start = 4,
            left_end = 5,
            right_start = 6,
            right_end = 7,
            top_start = 8,
            top_end = 9,
            bottom_start = 10,
            bottom_end = 11
        } struts_position_t;

      public:
        Struts();
        ~Struts();

        void init(Gtk::Window* window);
        void set_struts(bool force = false);
        void reset_struts();

        bool is_set() { return m_strut_set; }

      private:
        Gtk::Window* m_window;
        bool m_strut_set = false;
    };

}  // namespace docklight::position
