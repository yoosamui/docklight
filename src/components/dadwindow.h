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
#include <gdkmm/general.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "components/position.h"
#include "components/config.h"
// clang-format on

namespace docklight
{

    class DADWindow : public Gtk::Window
    {
      public:
        DADWindow(const Glib::RefPtr<Configuration>& config,
                  const Glib::RefPtr<PositionManager>& position, Glib::RefPtr<Gdk::Pixbuf> icon);
        ~DADWindow();

        void show_at(int dockitem_index);
        void move_at(int x, int y);
        void close_now();

        // int get_x() { return m_xoffset; }
        // int get_y() { return yoffset; }

      private:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

      private:
        Glib::RefPtr<Gdk::Pixbuf> m_icon;
        Glib::RefPtr<Configuration> m_config;
        Glib::RefPtr<PositionManager> m_position;

        int m_size = 0;

        int m_xoffset = 0;
        int m_yoffset = 0;
    };
}  // namespace docklight

