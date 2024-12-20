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
#include <thread>

#include <sigc++/sigc++.h>
#include <glibmm/main.h>
#include <gtkmm/window.h>
#include "utils/cairo.h"
#include <gdkmm/pixbuf.h>
#include "utils/wnck.h"
#include "utils/pixbuf.h"
#include "components/dockitemicon.h"

#include "components/autohidemanager.h"
// clang-format on

namespace docklight
{
    class PanelPreview : public Gtk::Window
    {
      public:
        PanelPreview();
        ~PanelPreview();

        void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y,
                       const std::string& text, bool indicator);
        void show_at(int dockitem_index, std::shared_ptr<DockItemIcon> dockitem);
        void hide_now();

        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool get_visible() const;

        guint get_width();
        guint get_size();

      private:
        void connect_signal(bool connect);
        bool on_timeout_draw();
        // bool on_timeout_close_window();

        // void thread_func();
        guint get_dockpreview_index(int mx, int my);
        guint get_scale_factor();

        bool on_enter_notify_event(GdkEventCrossing* crossing_event) override;
        bool on_leave_notify_event(GdkEventCrossing* crossing_event) override;

        bool on_button_press_event(GdkEventButton* event);
        bool on_button_release_event(GdkEventButton* event);
        bool on_motion_notify_event(GdkEventMotion* event) override;

        void update();

        void read_images();
        void on_container_updated(window_action_t action, glong xid);

        void thread_func();

      private:
        // Glib::RefPtr<AnimBoomWindow> m_anim;

        sigc::connection m_sigc_updated;
        sigc::connection m_sigc_connection;

        Gdk::Rectangle m_close_button_rectangle;
        Glib::RefPtr<Gdk::Pixbuf> m_image;

        std::vector<std::pair<Glib::RefPtr<Gdk::Pixbuf>, std::shared_ptr<DockItemIcon>>>
            m_current_images;

        // std::vector<std::pair<gint, std::shared_ptr<DockItemIcon>>> m_windows;
        std::shared_ptr<DockItemIcon> m_dockitem;
        //        std::shared_ptr<std::thread> m_bck_thread;

        //   WnckWindow* m_delete_pending_window = nullptr;
        std::mutex m_mutex;
        int m_last_dockpreview_index = 0;
        int m_dockitem_index = 0;
        int m_dockpreview_index = 0;
        int m_size = 0;
        int m_items = 0;
        bool m_block_leave = false;
        bool m_visible = false;
        //  bool m_anim_start = false;
        bool m_block_draw = false;
        int m_x = 0;
        int m_y = 0;

        Theme* m_theme = &Config()->get_theme();
    };

}  // namespace docklight
