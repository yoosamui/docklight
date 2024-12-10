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
#include <gdkmm/rectangle.h>
#include "components/config.h"

#include "utils/wnck.h"
#include "components/dockitemicon.h"
#include "dockitemprovider.h"
#include "components/position.h"
#include "components/panelrender.h"

#include <gdk/gdk.h>
#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/menuitem.h>

#include "components/animboomwindow.h"
#include "components/dadwindow.h"
//#include "components/panelbase.h"
#include "components/panelpreview.h"
#include "components/titlewindow.h"

// clang-format on

namespace docklight
{
    class Panel : public PanelRender
    {
      public:
        Panel();
        ~Panel();

        void init(const Glib::RefPtr<Gtk::Application>& app,
                  const Glib::RefPtr<Configuration>& config,
                  const Glib::RefPtr<DockItemProvider>& provider,
                  const Glib::RefPtr<PositionManager>& position);

        void container_updated(guint explicit_size = 0);

      private:
        sigc::connection m_sigc_draw;
        sigc::connection m_sigc_updated;

        guint get_scale_factor();
        guint get_dockitem_index(int mx, int my);

        bool on_button_press_event(GdkEventButton* event);
        bool on_button_release_event(GdkEventButton* event);

        bool on_motion_notify_event(GdkEventMotion* event);
        void on_container_updated(window_action_t action, int index);

        void on_autohide_before_hide(int tag);
        void on_autohide_after_hide(int tag);

        bool on_timeout_draw();

        bool on_enter_notify_event(GdkEventCrossing* crossing_event) override;
        bool on_leave_notify_event(GdkEventCrossing* crossing_event) override;

        bool on_scroll_event(GdkEventScroll* e) override;

        void on_item_menu_childlist_event(WnckWindow* window);

        static void on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data);

        // static void on_state_changed(WnckWindow* window, WnckWindowState changed_mask,
        // WnckWindowState new_state, gpointer user_data);

        // static void on_actions_changed(WnckWindow* window, WnckWindowActions changed_mask,
        // WnckWindowActions new_state, gpointer user_data);

        void thread_func();
        void show_current_title(bool show);
        void drag_drop(bool start);

      private:
        Glib::RefPtr<Configuration> m_config;
        Glib::RefPtr<AnimBoomWindow> m_composite;
        Glib::RefPtr<PanelPreview> m_preview;
        Glib::RefPtr<DockItemProvider> m_provider;
        Glib::RefPtr<PositionManager> m_position;
        DADWindow* m_dad = nullptr;

        std::shared_ptr<DockItemIcon> m_dockitem;

        Glib::RefPtr<TitleWindow> m_title;

        guint m_preview_open_index = 0;

        bool m_preview_open = false;
        bool m_mouse_move = false;
        bool m_mouse_press = false;

        float m_mouseclickEventTime = 0.f;

        Glib::Timer m_mouse_click_timer;
        Glib::Timer m_mouse_drag_drop_timer;

        bool m_drag_drop_starts = false;
        bool m_drag_drop_candrop = false;

        std::shared_ptr<std::thread> m_bck_thread;

        Gtk::SeparatorMenuItem* m_separatorMenuItem = nullptr;

        guint m_mouse_button = 0;
        guint m_last_index = 0;
        guint m_last_title_index = 0;

        gint m_scroll_index = 0;
    };
}  // namespace docklight

