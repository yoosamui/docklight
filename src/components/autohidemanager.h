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

#include <glibmm/main.h>
#include <glibmm/timer.h>
#include <gtkmm/window.h>
#include <sigc++/sigc++.h>

//#include "components/dockmenu.h"
#include "components/position.h"
#include "utils/easing.h"
#include "utils/wnck.h"

// clang-format on

namespace docklight
{

    class AutohideManager : public Glib::Object
    {
        typedef sigc::signal<void, int, int> type_signal_hide;
        typedef sigc::signal<void, int> type_signal_before_hide;
        typedef sigc::signal<void, int> type_signal_after_hide;

      public:
        AutohideManager();
        ~AutohideManager();

        bool get_intersects() const;

        void force_show();
        bool get_visible() const;

        bool get_autohide_allow() const;
        void set_hide_allow(bool allow);

        type_signal_hide signal_hide();
        type_signal_before_hide signal_before_hide();
        type_signal_after_hide signal_after_hide();

      private:
        bool on_autohide();

        void show_now();
        void hide_now();
        void get_offset(float position, int& offset_x, int& offset_y);

        void connect_signal_handler(bool connect);
        void connect_signal_hide(bool connect);

        bool on_hide();

        static bool is_window_intersect(WnckWindow* window);
        static void on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data);

      private:
        static WnckWindow* m_active_window;

        Glib::Timer m_autohide_timer;

        const float m_hide_delay = 40.f;
        const float m_show_delay = 10.f;

        bool m_visible = true;
        bool m_intersects = false;
        bool m_last_intersects = false;
        bool m_hide_allow = true;

        type_signal_hide m_signal_hide;
        type_signal_before_hide m_signal_before_hide;
        type_signal_after_hide m_signal_after_hide;

        sigc::connection m_sigc_autohide;
        sigc::connection m_sigc_hide;

        int m_offset_x = 0;
        int m_offset_y = 0;
        int m_animation_time = 0;
        int m_area = 0;

        float m_start_position = 0.f;
        float m_end_position = 0.f;
        float m_init_time = 0.f;
        float m_end_time = 0.f;
    };

    Glib::RefPtr<AutohideManager> create_autohide();
    Glib::RefPtr<AutohideManager> Autohide();

}  // namespace docklight
