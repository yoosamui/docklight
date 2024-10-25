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
#include <gtkmm/drawingarea.h>
#include <glibmm/timer.h>
#include <math.h>

#include "components/config.h"
#include "utils/easing.h" // for testing
#include  <thread>

#include "utils/wnck.h"
#include "components/dockitemicon.h"
#include "dockitemprovider.h"
#include "components/TransparentWindow.h"
#include "components/position.h"
#include "components/dockrender.h"
// clang-format on
//
#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/menuitem.h>

namespace docklight
{

    class Panel : public DockRender
    {
      public:
        Panel();
        ~Panel();

        void init(Glib::RefPtr<Gtk::Application> app);
        void container_updated(guint explicit_size = 0);

      private:
        Glib::RefPtr<Gtk::Application> m_app;
        sigc::connection m_sigc_draw;
        sigc::connection m_sigc_updated;

        guint get_dockitem_index(int mx, int my);

        guint get_scale_factor();
        bool on_button_press_event(GdkEventButton* event);
        bool on_motion_notify_event(GdkEventMotion* event);
        void on_container_updated(window_action_t action, int index);

        bool on_timeout_draw();

        bool on_enter_notify_event(GdkEventCrossing* crossing_event) override;
        bool on_leave_notify_event(GdkEventCrossing* crossing_event) override;

        Glib::Timer m_animation_timer;
        // Mouse handlers
        // bool on_button_press_event(GdkEventButton* event);
        // bool on_button_release_event(GdkEventButton* event);
        // bool on_motion_notify_event(GdkEventMotion* event);
        bool on_scroll_event(GdkEventScroll* e) override;

        // Menus
        void on_item_menu_childlist_event(WnckWindow* window);

        void on_home_menu_quit_event() override;

        // void on_home_menu_position(int& x, int& y, bool& push_in) override
        //{
        // DockMenu::on_home_menu_position(x, y, push_in);
        //}

        // void on_item_menu_position(int& x, int& y, bool& push_in) override
        //{
        // DockMenu::on_item_menu_position(x, y, push_in);
        //}
        //
        // Gtk::DrawingArea m_drawing_area;
        // void on_drawingarea(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);

        static void on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data);
        void thread_func();

      private:
        std::shared_ptr<std::thread> m_bck_thread;
        Gtk::SeparatorMenuItem* m_separatorMenuItem = nullptr;
        // std::thread* m_bck_thread = nullptr;
        Glib::RefPtr<DockItemProvider> m_provider;
        easing::bounce m_bounce;
        guint m_last_index = 0;
        gint m_scroll_index = 0;
    };
}  // namespace docklight

