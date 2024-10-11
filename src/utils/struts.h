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
        void set_insets(long& insets);
        //        void set_struts(bool force = false);
        void reset_struts();

        void set_struts(const guint pos = 0);

        guint get_bottom_pos() { return m_last_bottom_pos; }
        guint get_right_pos() { return m_last_right_pos; }
        guint get_left_pos() { return m_last_left_pos; }
        guint get_top_pos() { return m_last_top_pos; }

        bool active() { return m_active; }

      private:
        Gtk::Window* m_window;
        bool m_active = false;
        guint m_last_bottom_pos = 0;
        guint m_last_right_pos = 0;
        guint m_last_left_pos = 0;
        guint m_last_top_pos = 0;

        gint m_strut_top_pos = -1;
        gint m_strut_right_pos = -1;
        gint m_strut_left_pos = -1;
        gint m_strut_bottom_pos = -1;
    };

}  // namespace docklight::position
