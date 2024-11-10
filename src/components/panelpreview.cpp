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
#include <iostream>

#include "panelpreview.h"

#include "utils/system.h"
#include "translations.h"
// clang-format on

namespace docklight
{

    PanelPreview::PanelPreview() : Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
    {
        set_resizable(true);
        set_skip_taskbar_hint(true);
        set_skip_pager_hint(true);
        set_keep_above(true);

        m_size = Config()->get_preview_image_size();
    }

    PanelPreview::~PanelPreview()
    {
        connect_signal(false);
        g_message(MSG_FREE_OBJECT, "PanelPreview");
    }

    void PanelPreview::connect_signal(bool connect)
    {
        if (connect) {
            m_sigc_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &PanelPreview::on_timeout_draw), 1000 / 4);
        } else {
            m_sigc_connection.disconnect();
        }
    }

    bool PanelPreview::on_timeout_draw()

    {
        Gtk::Widget::queue_draw();
        return true;
    }

    guint PanelPreview::get_width()
    {
        return m_size;
    }

    void PanelPreview::show_at(int x, int y, std::shared_ptr<DockItemIcon> dockitem)
    {
        m_dockitem = dockitem;
        const int millis = 120;
        int event_time = gtk_get_current_event_time();

        GdkScreen* screen = gdk_screen_get_default();
        int current_ws_number = gdk_x11_screen_get_current_desktop(screen);

        m_current_images.clear();
        if (!system::is_mutter_window_manager()) {
            for (auto& it : m_dockitem->get_childmap()) {
                auto child = it.second;
                auto xid = it.first;
                bool restore = false;

                WnckWindow* window = it.second->get_wnckwindow();

                auto ws = wnck_window_get_workspace(window);
                if (WNCK_IS_WORKSPACE(ws)) wnck_workspace_activate(ws, event_time);

                if (wnck_window_is_minimized(window)) {
                    wnck::select_window(window);
                    restore = true;
                }

                if (wnck_window_is_pinned(window)) {
                    wnck_window_unpin(window);
                    restore = true;
                }

                std::string wstringx = "";
                if (wnck::count_in_workspace(window, wstringx)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
                } else if (wnck_window_is_minimized(window)) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
                }

                pixbuf::get_window_image(xid, m_image);
                if (pixbuf::get_window_image(xid, m_image)) m_current_images.push_back(m_image);

                if (restore) {
                    wnck::minimize(window);
                }
            }

            auto cws = wnck_screen_get_workspace(wnck::get_default_screen(), current_ws_number);

            if (WNCK_IS_WORKSPACE(cws)) {
                wnck_workspace_activate(cws, 1);
            }
        } else {
            for (auto& it : m_dockitem->get_childmap()) {
                auto xid = it.first;
                if (pixbuf::get_window_image(xid, m_image)) {
                    m_current_images.push_back(m_image);
                }
            }
        }
        auto size = dockitem->get_childmap().size();

        show();
        resize(m_size * size, m_size);
        move(x, y);
        m_visible = true;
    }

    bool PanelPreview::get_visible() const
    {
        return m_visible;
    }
    void PanelPreview::hide_now()
    {
        connect_signal(false);
        hide();
        m_visible = false;
    }
    bool PanelPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        cr->set_source_rgba(0.266, 0.309, 0.361, 1.0);
        cairo::rounded_rectangle(cr, 0, 0, m_size, m_size, 4.0);
        cr->fill();

        int startX = 0;
        int startY = 0;

        for (auto& it : m_current_images) {
            Glib::RefPtr<Gdk::Pixbuf> image = it;

            Gdk::Cairo::set_source_pixbuf(cr, image, startX, startY);
            cr->paint();

            startX += m_size;
            startY = 0;
        }

        return true;
    }
}  // namespace docklight
