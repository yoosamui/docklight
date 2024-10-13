#pragma once
//  Copyright(c) 2018 - 2024 Juan R.González
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
#include "utils/struts.h"
// clang-format on

namespace docklight
{
    class PositionManager : public Glib::Object
    {
      public:
        PositionManager(Gtk::Window* window);

        const Gtk::Window* get_window() const;
        Gdk::Rectangle get_workarea() const;
        Gdk::Rectangle get_monitor() const;
        Gdk::Rectangle get_window_geometry() const;

        // double get_window_scalling_factor(const guint items_count, const guint area);
        // double get_scalling_factor(const guint screen_width, const guint item_size,
        // const guint size);

        void monitor_changed();
        void force_position();
        void reset_position();
        void set_position(guint required_size);

      private:
        position::Struts m_struts;
        Gtk::Window* m_window;
        guint m_last_required_size = 0;

        guint m_width = 1;
        guint m_height = 1;
        guint m_y = 0;
        guint m_x = 0;
    };

    Glib::RefPtr<PositionManager> create_position(Gtk::Window* window);
    Glib::RefPtr<PositionManager> Position();

}  // namespace docklight
