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

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "appobserver.h"

namespace docklight
{
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

    void AppObserver::on_icon_changed(WnckWindow* window, gpointer user_data)
    {
        g_print("on_icon_chnaged!");
    }

    void AppObserver::on_actions_changed(WnckWindow* window, WnckWindowActions changed_mask,
                                         WnckWindowActions new_state, gpointer user_data)
    {
        //
    }

    // void AppObserver::on_active_window_changed(WnckScreen* screen,
    // WnckWindow* previously_active_window,
    // gpointer user_data)
    //{
    // static gulong m_state_change_id = 0;
    // WnckWindow* active_window = wnck_screen_get_active_window(screen);
    // if (!active_window) {
    // return;
    //}

    // WnckWindowType wt = wnck_window_get_window_type(active_window);
    // if (wt == WNCK_WINDOW_DESKTOP) {
    // return;
    //}

    // if (previously_active_window && m_state_change_id) {
    // g_signal_handler_disconnect(previously_active_window, m_state_change_id);
    //}

    // m_state_change_id = g_signal_connect_after(
    // active_window, "state-changed", G_CALLBACK(AppObserver::on_state_changed), nullptr);
    //}

    void AppObserver::on_active_workspace_changed_callback(WnckScreen* screen,
                                                           WnckWorkspace* previously_active_space,
                                                           gpointer user_data)
    {
        WnckWindow* active_window = wnck_screen_get_active_window(screen);
        if (!active_window) {
            return;
        }

        Provider()->workspace_change();
    }

    // void AppObserver::on_state_changed(WnckWindow* window, WnckWindowState changed_mask,
    // WnckWindowState new_state, gpointer user_data)

    //{
    // bool fullscreen = wnck_window_is_fullscreen(window);
    // g_print("FULLSCREEN %s\n", fullscreen ? "yes" : "no");

    // Position()->fullscreen_change(fullscreen);
    //}

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

        gulong xid = wnck_window_get_xid(window);

        if (Provider()->exist(xid)) {
            return;
        }

        Provider()->insert(window);

        /*auto instance_name = wnck_window_get_class_instance_name(window);
        auto group_name = wnck_window_get_class_group_name(window);
        g_message("->%s/ %s", instance_name, group_name);*/

        // Glib::RefPtr<Gdk::Pixbuf> image;
        // for (int i = 0; i < 2; i++)
        // if (pixbuf::get_window_image(xid, image)) {
        // char filename[512];
        // sprintf(filename, "/home/yoo/TEMP/docklight_icons/%lu, %s-%s", xid, instance_name,
        // group_name);

        ////   image->save(filename, "png");

        ////  g_message("---->SET IMAGE %s-%s\n", instance_name, group_name);
        //} else {
        // g_message("ERROR->SET IMAGE %s-%s\n", instance_name, group_name);
        //}

        /*size_t idx = 1;
        for (; idx < Provider()->data().size(); idx++) {
            auto dockitem = Provider()->data().at(idx);
            auto xid_list = dockitem->get_wnck_xid_list();
            //            gulong cxid = wnck_window_get_xid(window);

            if (std::find(xid_list.begin(), xid_list.end(), xid) != xid_list.end()) {
                for (auto& it : dockitem->get_childmap()) {
                    auto child = it.second;

                    if (child->get_xid() == xid) {
                        Glib::RefPtr<Gdk::Pixbuf> image;

                        // if (wnck_window_is_minimized(window))
                        //  wnck::unminimize(window);
                        wnck_window_make_below(window);
                        //        wnck::select_window(child->get_wnckwindow());

                        if (pixbuf::get_window_image(xid, image)) {
                            child->set_image(image);
                            char filename[512];
                            sprintf(filename, "/home/yoo/TEMP/docklight_icons/%s-%s",
                                    dockitem->get_instance_name().c_str(),
                                    dockitem->get_group_name().c_str());

                            image->save(filename, "png");

                            g_message("---->SET IMAGE %s\n", dockitem->get_group_name().c_str());
                            break;
                        }
                    }
                }
                break;
            }
        }*/
    }

    AppObserver::AppObserver()
    {
        WnckScreen* wnckscreen = wnck::get_default_screen();

        g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                         G_CALLBACK(&AppObserver::on_window_opened), nullptr);

        g_signal_connect(G_OBJECT(wnckscreen), "window-closed",
                         G_CALLBACK(&AppObserver::on_window_closed), nullptr);

        g_signal_connect(wnckscreen, "active-workspace-changed",
                         G_CALLBACK(AppObserver::on_active_workspace_changed_callback), nullptr);

        // g_signal_connect(wnckscreen, "active_window_changed",
        // G_CALLBACK(AppObserver::on_active_window_changed), nullptr);

        // g_signal_connect(G_OBJECT(wnckscreen), "icon-changed",
        // G_CALLBACK(&AppObserver::on_icon_changed), nullptr);

        // g_signal_connect(G_OBJECT(wnckscreen), "actions-changed",
        // G_CALLBACK(&AppObserver::on_actions_changed), nullptr);
        g_message("Create AppObserver.");
    }

}  // namespace docklight
