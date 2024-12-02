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
#include <glibmm/main.h>
#include <gtkmm/window.h>

// clang-format on

#define DF_EXPLODES_FRAMESPERSECOND 30
#define DF_EXPLODES_FRAMERATE double(1000 / DF_EXPLODES_FRAMESPERSECOND)
#define DF_EXPLODES_TIMEMAX double(DF_EXPLODES_FRAMESPERSECOND * 10000)

namespace docklight
{
    class AnimBoomWindow : public Gtk::Window
    {
      public:
        AnimBoomWindow();
        ~AnimBoomWindow();

        void show_at(int x, int y);
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

      private:
        void connect_signal(bool connect);
        bool on_timeout_draw();

      private:
        sigc::connection m_sigc_connection;
        Glib::RefPtr<Gdk::Pixbuf> m_image;

        long int m_start_time = 0;
        long int m_frame_time = 0;

        int m_size = 0;
        int m_frames = 0;
    };

}  // namespace docklight
