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

#include "appobserver.h"

namespace docklight
{

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
    void AppObserver::on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        if (!wnck::is_valid_window_type(window)) return;

        gint32 xid = wnck_window_get_xid(window);
        Provider()->remove(xid);
    }

    void AppObserver::on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        if (!wnck::is_valid_window_type(window)) return;

        gint32 xid = wnck_window_get_xid(window);

        if (Provider()->exist(xid)) {
            return;
        }

        Provider()->insert(window);
    }

    AppObserver::AppObserver()
    {
        WnckScreen* wnckscreen = wnck::get_default_screen();

        g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                         G_CALLBACK(&AppObserver::on_window_opened), nullptr);

        g_signal_connect(G_OBJECT(wnckscreen), "window-closed",
                         G_CALLBACK(&AppObserver::on_window_closed), nullptr);

        g_message("Create AppObserver.");
    }

    Glib::RefPtr<AppObserver> m_observer;
    Glib::RefPtr<AppObserver> create_observer()
    {
        if (!m_observer) {
            m_observer = Glib::RefPtr<AppObserver>(new AppObserver());
        }

        return m_observer;
    }

    Glib::RefPtr<AppObserver> Observer()
    {
        g_assert(m_observer);
        return m_observer;
    }

}  // namespace docklight
