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
#include <glibmm/main.h>
#include <gtkmm/window.h>
#include "utils/cairo.h"
#include <gdk/gdkx.h>
#include "utils/wnck.h"
#include "utils/pixbuf.h"
#include "components/dockitemicon.h"
// clang-format on
#include "X11/X.h"
namespace docklight
{
    class CoverWindow : public Gtk::Window  // TransparentWindow
    // class CoverWindow : public TransparentWindow
    {
      public:
        CoverWindow();
        ~CoverWindow();

        void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y,
                       const std::string& text);
        void show_at(int x, int y, std::shared_ptr<DockItemIcon> dockitem);
        void hide_now();

        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool get_visible() const;

        guint get_width();

      private:
        void connect_signal(bool connect);
        bool on_timeout_draw();
        guint get_dockpreview_index(int mx, int my);

        // bool on_enter_notify_event(GdkEventCrossing* crossing_event) override;
        // bool on_leave_notify_event(GdkEventCrossing* crossing_event) override;

        // bool on_button_press_event(GdkEventButton* event);
        // bool on_button_release_event(GdkEventButton* event);
        //  bool on_motion_notify_event(GdkEventMotion* event) override;

      private:
        sigc::connection m_sigc_connection;
        Glib::RefPtr<Gdk::Pixbuf> m_image;
        std::vector<std::pair<Glib::RefPtr<Gdk::Pixbuf>, std::shared_ptr<DockItemIcon>>>
            m_current_images;

        std::shared_ptr<DockItemIcon> m_dockitem;

        int m_dockpreview_index = 0;
        int m_size = 0;
        bool m_visible = false;
    };

}  // namespace docklight
