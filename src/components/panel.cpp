#pragma GCC diagnostic ignored "-Wunused-parameter"
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

//#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/i18n.h>
#include <iostream>
#include <cmath>

#include "components/panel.h"
#include "translations.h"
#include "components/position.h"
// clang-format on

namespace docklight
{
    Panel::Panel()
    {
        // clang-format off
        // Set event masks
        add_events( Gdk::BUTTON_PRESS_MASK |
                    Gdk::BUTTON_RELEASE_MASK |
                    Gdk::SCROLL_MASK |
                    Gdk::ENTER_NOTIFY_MASK |
                    Gdk::LEAVE_NOTIFY_MASK |
                    Gdk::POINTER_MOTION_MASK
                   );
        // clang-format on

        m_provider = Provider();
        g_message("Create Panel.");
    }

    void Panel::init(Glib::RefPtr<Gtk::Application> app)
    {
        m_app = app;
        m_sigc_updated =
            m_provider->signal_update().connect(sigc::mem_fun(this, &Panel::on_container_updated));

        WnckScreen* wnckscreen = wnck::get_default_screen();
        g_signal_connect(wnckscreen, "active_window_changed",
                         G_CALLBACK(Panel::on_active_window_changed), nullptr);

        m_position = Position();
        m_bck_thread = std::shared_ptr<std::thread>(new std::thread(&Panel::thread_func, this));

        m_composite = Glib::RefPtr<ExplodesWindow>(new ExplodesWindow());
        m_preview = Glib::RefPtr<PanelPreview>(new PanelPreview());
    }

    Panel::~Panel()
    {
        m_bck_thread->detach();

        m_sigc_updated.disconnect();
        g_message(MSG_FREE_OBJECT, "Panel");
    }

    void Panel::thread_func()
    {
        WnckWindow* window = nullptr;
        while (true) {
            if (window != m_active_window) {
                window = m_active_window;

                if (!window) continue;

                size_t idx = 1;
                for (; idx < m_provider->data().size(); idx++) {
                    auto dockitem = m_provider->data().at(idx);
                    auto xid_list = dockitem->get_wnck_xid_list();
                    gulong xid = wnck_window_get_xid(window);

                    if (std::find(xid_list.begin(), xid_list.end(), xid) != xid_list.end()) {
                        m_dockitem_active_index = idx;
                        Gtk::Widget::queue_draw();

                        break;
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void Panel::on_active_window_changed(WnckScreen* screen, WnckWindow* previously_active_window,
                                         gpointer user_data)
    {
        // Gets the active WnckWindow on screen.
        // May return NULL sometimes, since not all
        // window managers guarantee that a window is always active.
        m_active_window = wnck_screen_get_active_window(screen);
    }

    bool Panel::on_enter_notify_event(GdkEventCrossing* crossing_event)
    {
        //     m_sigc_draw =
        //         Glib::signal_timeout().connect(sigc::mem_fun(this, &Panel::on_timeout_draw),
        //         1000 / 8);

        m_mouse_enter = true;
        return false;
    }

    bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
    {
        //       m_sigc_draw.disconnect();

        m_mouse_enter = false;
        Gtk::Widget::queue_draw();
        return false;
    }

    bool Panel::on_timeout_draw()
    {
        return true;
    }

    void Panel::on_home_menu_quit_event()
    {
        m_app->quit();
    }

    inline guint Panel::get_scale_factor()
    {
        const auto workarea = device::monitor::get_workarea();
        const int max_icon_size = Config()->get_custom_icon_size();
        const int num_items = m_provider->data().size();
        const int item_width = max_icon_size;
        int screen_width = 0;

        screen_width = Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL
                           ? workarea.get_width()
                           : workarea.get_height();

        // Calculate the scaling factor
        float scaling_factor =
            static_cast<float>(screen_width) / static_cast<float>(num_items * item_width);

        int icon_size = std::floor(item_width * scaling_factor);

        if (icon_size > max_icon_size) {
            icon_size = max_icon_size;
        };

        return icon_size - Config()->get_dock_area_margin();
    }

    void Panel::container_updated(guint explicit_size)
    {
        auto size = m_provider->data().size();
        auto separator_size = Config()->get_separator_size();
        auto separators_count = (size * separator_size);

        // resize the icon if necesery
        int scaled_icon_size = get_scale_factor();
        Config()->set_icon_size(scaled_icon_size);

        // draw and positioning
        auto required_size = m_provider->required_size(separators_count);
        m_position->set_position(required_size);
    }

    void Panel::on_container_updated(window_action_t action, int index)
    {
        container_updated();
        Gtk::Widget::queue_draw();
        g_message("Panel updated");
    }

    bool Panel::on_motion_notify_event(GdkEventMotion* event)
    {
        if (!m_mouse_enter) return true;

        get_dockitem_index(event->x, event->y);

        if (m_dockitem_index != m_last_index) {
            m_last_index = m_dockitem_index;
            Gtk::Widget::queue_draw();
        }

        // if (m_preview && m_preview_open && m_preview_open_index &&
        //(m_dockitem_index > m_preview_open_index + 1 ||
        // m_dockitem_index < m_preview_open_index - 1)) {
        ////
        // m_preview->hide_now();
        //}

        // stop other handlers from being invoked for the event.
        return true;
    }

    inline guint Panel::get_dockitem_index(int mx, int my)
    {
        gint pos_x = 0;
        gint pos_y = 0;

        auto separator_size = Config()->get_separator_size();
        auto area = Config()->get_dock_area() + separator_size;
        auto size = m_provider->data().size();
        auto maxsize = size * area;
        auto start_pos = 0;

        get_start_pos(maxsize, pos_x, pos_y);

        for (size_t idx = 0; idx < size; idx++) {
            m_dockitem_index = -1;
            if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                if (mx >= pos_x && mx <= pos_x + area) {
                    m_dockitem_index = idx;
                    break;
                }
                pos_x += start_pos + area;

            } else {  // Vertical
                if (my >= pos_y && my <= pos_y + area) {
                    m_dockitem_index = idx;
                    break;
                }
                pos_y += start_pos + area;
            }
        }

        return m_dockitem_index;
    }

    bool Panel::on_scroll_event(GdkEventScroll* e)
    {
        if (m_dockitem_index < 1) return false;

        std::shared_ptr<DockItemIcon> dockitem;
        if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;

        auto size = dockitem->get_childmap().size();
        if (!size) return false;

        if (e->direction == GDK_SCROLL_UP) {
            m_scroll_index++;
        } else if (e->direction == GDK_SCROLL_DOWN) {
            m_scroll_index--;
        }

        if (m_scroll_index < 0) {
            m_scroll_index = (int)size - 1;
        } else if (m_scroll_index > (int)size - 1) {
            m_scroll_index = 0;
        }

        auto it = dockitem->get_childmap().begin();
        std::advance(it, m_scroll_index);
        auto child = it->second;

        WnckWindow* window = child->get_wnckwindow();
        if (!window) return false;
        wnck::bring_above_window(window);

        return true;
    }

    void Panel::on_item_menu_childlist_event(WnckWindow* window)
    {
        wnck::activate_window(window);
    }

    bool Panel::on_button_press_event(GdkEventButton* event)
    {
        if ((event->type != GDK_BUTTON_PRESS)) return false;

        m_preview->hide_now();
        m_preview_open_index = 0;

        m_mouseclickEventTime = gtk_get_current_event_time();
        get_dockitem_index(event->x, event->y);

        // Handle the left mouse button.
        if (event->button == 1 && m_dockitem_index > 0) {
            std::shared_ptr<DockItemIcon> dockitem;
            if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;

            auto size = dockitem->get_childmap().size();

            if (!size) {
                dockitem->launch();
                return true;
            }

            wnck::select_window(dockitem->get_hash(), m_active_window,
                                dockitem->get_wnck_window_list());
            //     return true;
        }
        return true;
    }

    bool Panel::on_button_release_event(GdkEventButton* event)
    {
        if ((event->type != GDK_BUTTON_RELEASE)) return false;

        std::shared_ptr<DockItemIcon> dockitem;
        if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;

        auto size = dockitem->get_childmap().size();
        //  int required = system::is_mutter_window_manager() ? 1 : size;

        // Handle the preview right mouse button.
        if (size && event->button == 3 && !m_preview->get_visible()) {
            int diff = (int)((gtk_get_current_event_time() - m_mouseclickEventTime));
            if (diff > 200 && m_dockitem_index > 0) {
                int x = 0;
                int y = 0;

                std::shared_ptr<DockItemIcon> dockitem;
                if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;
                // int area = Config()->get_preview_area();
                // int size = area * dockitem->get_childmap().size();
                // Position()->get_preview_position(m_dockitem_index, x, y, size, area);

                m_preview->show_at(x, y, m_dockitem_index, dockitem);
                m_preview_open_index = m_dockitem_index;
                m_preview_open = true;
                return true;
            }
        }

        if (m_preview_open) {
            m_preview_open = false;
            return true;
        }

        // Handle the right mouse button.
        if (event->button == 3) {
            if (m_dockitem_index == 0) {
                // Home Menu
                if (!m_home_menu.get_attach_widget()) {
                    m_home_menu.attach_to_widget(*this);
                }

                auto count = wnck::get_windows_count();
                m_HomeMinimizeAllWindowsMenuItem.set_sensitive(count);
                m_HomeUnMinimizeAllWindowsMenuItem.set_sensitive(count);
                m_home_menu_close_all_item.set_sensitive(count);

                m_home_menu.popup(sigc::mem_fun(*this, &Panel::on_home_menu_position), 1,
                                  event->time);

            } else if (m_dockitem_index > 0) {
                // Items Menu
                m_item_menu_attach.set_active(dockitem->get_attached());

                auto size = dockitem->get_childmap().size();

                m_item_menu_close_all.set_sensitive(size);
                m_item_menu_minimize_all.set_sensitive(size);
                m_item_menu_unminimize_all.set_sensitive(size);

                // populate childrens;
                for (auto& itemMenu : m_item_menu.get_children()) {
                    auto type = dynamic_cast<Gtk::ImageMenuItem*>(itemMenu);
                    if (type) {
                        m_item_menu.remove(*itemMenu);
                    }
                }

                if (m_separatorMenuItem) m_item_menu.remove(*m_separatorMenuItem);

                m_separatorMenuItem = Gtk::manage(new Gtk::SeparatorMenuItem());
                m_item_menu.append(*m_separatorMenuItem);

                for (auto& item : dockitem->get_childmap()) {
                    auto child = item.second;

                    Gtk::ImageMenuItem* menu_item =
                        Gtk::manage(new Gtk::ImageMenuItem(child->get_title()));

                    const Glib::RefPtr<Gdk::Pixbuf> pixbuf = child->get_icon(16);
                    Gtk::Image* image = Gtk::manage(new Gtk::Image(pixbuf));

                    menu_item->set_image(*image);
                    menu_item->set_always_show_image(true);
                    menu_item->set_label(child->get_window_name());

                    std::string wstring;
                    if (wnck::count_in_workspace(child->get_wnckwindow(), wstring)) {
                        Glib::ustring label = wstring + child->get_window_name();
                        menu_item->set_label(label);
                    }

                    menu_item->signal_activate().connect(sigc::bind<WnckWindow*>(
                        sigc::mem_fun(*this, &Panel::on_item_menu_childlist_event),
                        child->get_wnckwindow()));

                    m_item_menu.append(*menu_item);
                }

                m_item_menu.show_all();
                m_item_menu.popup_at_pointer(nullptr);
                m_item_menu.popup(sigc::mem_fun(*this, &Panel::on_item_menu_position), 0,
                                  event->time);
            }
        }

        return true;
    }

}  // namespace docklight

