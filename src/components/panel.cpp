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
#include "components/panel.h"
#include "translations.h"
// clang-format on

namespace docklight
{
    Panel::Panel()
    {
        // clang-format off
        add_events( Gdk::BUTTON_PRESS_MASK |
                    Gdk::BUTTON_RELEASE_MASK |
                    Gdk::SCROLL_MASK |
                    Gdk::ENTER_NOTIFY_MASK |
                    Gdk::LEAVE_NOTIFY_MASK |
                    Gdk::POINTER_MOTION_MASK
                   );
        // clang-format on

        m_title = Glib::RefPtr<TitleWindow>(new TitleWindow());
        m_provider = m_provider;

        m_panel_hide.signal_before_hide().connect(
            sigc::mem_fun(this, &Panel::on_autohide_before_hide));

        m_panel_hide.signal_after_hide().connect(
            sigc::mem_fun(this, &Panel::on_autohide_after_hide));

        g_message("Create Panel.");
    }

    void Panel::init(const Glib::RefPtr<Gtk::Application>& app,
                     const Glib::RefPtr<Configuration>& config,
                     const Glib::RefPtr<DockItemProvider>& provider,
                     const Glib::RefPtr<PositionManager>& position)
    {
        // TODO::
        m_composite = Glib::RefPtr<AnimBoomWindow>(new AnimBoomWindow());
        m_preview = Glib::RefPtr<PanelPreview>(new PanelPreview());

        m_app = app;
        m_config = config;
        m_provider = provider;
        m_position = position;

        m_sigc_updated =
            m_provider->signal_update().connect(sigc::mem_fun(this, &Panel::on_container_updated));

        WnckScreen* wnckscreen = wnck::get_default_screen();
        g_signal_connect(wnckscreen, "active_window_changed",
                         G_CALLBACK(Panel::on_active_window_changed), nullptr);

        // g_signal_connect(wnckscreen, "active_window_changed",
        // G_CALLBACK(Panel::on_active_window_changed), nullptr);

        m_bck_thread = std::shared_ptr<std::thread>(new std::thread(&Panel::thread_func, this));
    }

    Panel::~Panel()
    {
        m_bck_thread->detach();

        m_sigc_draw.disconnect();
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

        if (!m_active_window) {
            return;
        }

        Provider()->set_window_image(m_active_window);
    }

    bool Panel::on_enter_notify_event(GdkEventCrossing* crossing_event)
    {
        if (!m_panel_hide.get_visible()) {
            int x = 0;
            int y = 0;
            auto winrect = Position()->get_window_geometry();

            if (!system::get_mouse_position(x, y)) return true;

            if (y > winrect.get_y()) {
                m_panel_hide.force_show();
            }

            return true;
        }

        m_sigc_draw =
            Glib::signal_timeout().connect(sigc::mem_fun(this, &Panel::on_timeout_draw), 1000 / 2);

        return false;
    }

    bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
    {
        /*if (m_panel_hide.get_visible() && m_panel_hide.get_intersects() && !m_context_menu_active
        && !m_preview_open) { m_panel_hide.force_hide();

            show_current_title(false);

            return true;
        }*/

        if (!m_drag_drop_starts) m_sigc_draw.disconnect();

        m_mouse_drag_drop_timer.stop();

        m_preview_open = false;
        m_mouse_press = false;
        m_mouse_enter = false;

        show_current_title(false);

        Gtk::Widget::queue_draw();
        return false;
    }

    void Panel::show_current_title(bool show)
    {
        if (show) {
            if (!m_provider->get_dockitem_by_index(m_dockitem_index, m_dockitem)) return;

            int size = m_dockitem->get_childmap().size();
            std::string extend = size ? " (" + std::to_string(size) + ")" : "";
            std::string title = m_dockitem->get_title() + extend;

            m_title->set_text(title);
            int width = m_title->get_width();
            int area = m_config->get_dock_area();

            int centerX, centerY;
            int x, y;

            if (m_config->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                int min = std::min(width, area);
                int max = std::max(width, area);

                centerX = max / 2 - min / 2;
                if (width == min) {
                    centerX = (min / 2 - max / 2);
                }

                x = (Position()->get_x() + (m_dockitem_index * area)) - centerX;
                y = Position()->get_y() - 50;

                if (m_config->get_dock_location() == dock_location_t::top) {
                    y = Position()->get_y() + 60;
                }
            } else {
                int min = std::min(m_title->get_height(), area);
                int max = std::max(m_title->get_height(), area);

                centerY = max / 2 - min / 2;
                y = Position()->get_y() + (m_dockitem_index * m_config->get_dock_area()) + centerY;

                if (m_config->get_dock_location() == dock_location_t::left) {
                    x = Position()->get_x() + area;
                } else {
                    x = Position()->get_x() - width;
                }
            }

            m_title->show_at(x, y);
        } else {
            m_title->hide_now();
        }
    }

    void Panel::drag_drop(bool start)
    {
        if (start && !m_drag_drop_starts) {
            std::shared_ptr<DockItemIcon> dockitem;
            if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return;

            dockitem->set_visible(false);
            Glib::RefPtr<Gdk::Pixbuf> icon = dockitem->get_icon(m_config->get_icon_size());

            m_dad = new DADWindow(m_config, Position(), icon);
            m_dad->show_at(m_dockitem_index);

            m_drag_drop_starts = true;
            m_drag_drop_item_index = m_dockitem_index;

            // m_title->hide_now();
            Gtk::Widget::queue_draw();

        } else {
            if (m_dad) {
                delete m_dad;
                m_dad = nullptr;
            }
            m_drag_drop_starts = false;
        }
    }

    bool Panel::on_timeout_draw()
    {
        if (m_mouse_button == 1 && m_mouse_press && !m_drag_drop_starts && m_dockitem_index &&
            m_mouse_drag_drop_timer.elapsed() > 0.2) {
            drag_drop(true);
        }

        return true;
    }

    inline guint Panel::get_scale_factor()
    {
        const auto workarea = device::monitor::get_workarea();
        const int max_icon_size = m_config->get_custom_icon_size();
        const int num_items = m_provider->data().size();
        const int item_width = max_icon_size;

        int screen_width = m_config->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL
                               ? workarea.get_width()
                               : workarea.get_height();

        // Calculate the scaling factor
        float scaling_factor =
            static_cast<float>(screen_width) / static_cast<float>(num_items * item_width);

        int icon_size = std::floor(item_width * scaling_factor);

        if (icon_size > max_icon_size) {
            icon_size = max_icon_size;
        };

        return icon_size - m_config->get_dock_area_margin();
    }

    void Panel::container_updated(guint explicit_size)
    {
        auto size = m_provider->data().size();
        auto separator_size = m_config->get_separator_size();
        auto separators_count = (size * separator_size);

        // resize the icon if necesery
        int scaled_icon_size = get_scale_factor();
        m_config->set_icon_size(scaled_icon_size);

        // draw and positioning
        auto required_size = m_provider->required_size(separators_count);
        m_position->set_position(required_size);
    }

    void Panel::on_container_updated(window_action_t action, int index)
    {
        if (action != window_action_t::UPDATE) return;

        container_updated();
        Gtk::Widget::queue_draw();
        g_message("Panel updated");
    }

    inline guint Panel::get_dockitem_index(int mx, int my)
    {
        gint pos_x = 0;
        gint pos_y = 0;

        auto separator_size = m_config->get_separator_size();
        auto area = m_config->get_dock_area() + separator_size;
        auto size = m_provider->data().size();
        auto maxsize = size * area;
        auto start_pos = 0;

        DockRender::get_start_pos(maxsize, pos_x, pos_y);

        for (size_t idx = 0; idx < size; idx++) {
            m_dockitem_index = -1;
            if (m_config->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
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

    void Panel::on_autohide_before_hide(int tag)
    {
        // TODO : allready exitst in  mouse release create a method
        m_preview->hide_now();
        m_preview_open = false;
        m_preview_open_index = 0;
        m_mouseclickEventTime = 0;
        m_preview->hide_now();
        m_preview_open_index = 0;

        m_home_menu.hide();
        m_item_menu.hide();
    }

    void Panel::on_autohide_after_hide(int tag) {}

    bool Panel::on_motion_notify_event(GdkEventMotion* event)
    {
        if (!m_panel_hide.get_visible()) {
            return true;
        }

        get_dockitem_index(event->x, event->y);

        if (m_mouse_button == 1 && m_dad && m_drag_drop_starts) {
            int x = 0;
            int y = 0;
            system::get_mouse_position(x, y);
            m_dad->move_at(x, y);

            if (m_config->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                m_drag_drop_candrop = (y + m_dad->get_height()) > Position()->get_y();
            } else {
                m_drag_drop_candrop = (x + m_dad->get_width()) > Position()->get_x();
            }
        }

        // show the title window
        if (!m_drag_drop_starts && !m_preview_open) show_current_title(true);

        if (m_dockitem_index != m_last_index) {
            m_last_index = m_dockitem_index;
            Gtk::Widget::queue_draw();
        }

        // stop other handlers from being invoked for the event.
        return true;
    }

    bool Panel::on_button_press_event(GdkEventButton* event)
    {
        if (!m_panel_hide.get_visible()) return true;
        if ((event->type != GDK_BUTTON_PRESS)) return false;
        get_dockitem_index(event->x, event->y);

        m_mouse_button = event->button;
        m_mouse_press = true;

        if (m_drag_drop_starts) return false;

        if (event->button == 1 && m_dockitem_index) {
            m_drag_drop_item_index = m_dockitem_index;
            m_mouse_drag_drop_timer.start();
        }

        if (m_preview_open) {
            m_mouseclickEventTime = 0;
            return true;
        }

        m_preview->hide_now();
        m_preview_open_index = 0;

        m_mouseclickEventTime = gtk_get_current_event_time();
        get_dockitem_index(event->x, event->y);

        return true;
    }

    bool Panel::on_button_release_event(GdkEventButton* event)
    {
        if (!m_panel_hide.get_visible()) return true;
        if ((event->type != GDK_BUTTON_RELEASE)) return false;

        get_dockitem_index(event->x, event->y);
        m_mouse_press = false;

        // stops drag & drop
        if (m_drag_drop_starts) {
            // reset
            m_drag_drop_starts = false;
            drag_drop(false);
            m_mouse_drag_drop_timer.stop();
            m_sigc_draw.disconnect();

            // drop if is posible.
            std::shared_ptr<DockItemIcon> dockitem;
            if (m_provider->get_dockitem_by_index(m_drag_drop_item_index, dockitem)) {
                if (m_drag_drop_candrop && m_dockitem_index &&
                    m_dockitem_index != m_drag_drop_item_index) {
                    m_drag_drop_candrop = false;

                    m_provider->drop(m_drag_drop_item_index, m_dockitem_index);
                }

                dockitem->set_visible(true);
                Gtk::Widget::queue_draw();
                return true;
            }
        }
        // Handle the left mouse button.
        if (event->button == 1 && m_dockitem_index) {
            std::shared_ptr<DockItemIcon> dockitem;
            if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;

            auto size = dockitem->get_childmap().size();

            if (!size) {
                dockitem->launch();
                return true;
            }

            wnck::select_window(dockitem->get_hash(), m_active_window,
                                dockitem->get_wnck_window_list());
            return true;
        }

        std::shared_ptr<DockItemIcon> dockitem;
        if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;
        int diff = (int)((gtk_get_current_event_time() - m_mouseclickEventTime));
        auto size = dockitem->get_childmap().size();

        if (m_preview_open) {
            m_preview->hide_now();
            m_preview_open = false;
            m_preview_open_index = 0;
            m_mouseclickEventTime = 0;  // gtk_get_current_event_time();

            return true;
        }
        if (!m_preview_open && event->button == 3) {
            // open preview
            if (size) {
                if (diff > 200 && m_dockitem_index > 0) {
                    m_preview->show_at(m_dockitem_index, dockitem);
                    m_preview_open_index = m_dockitem_index;
                    m_preview_open = true;

                    return true;
                }
            }

            // handle home menu
            if (m_dockitem_index == 0) {
                if (!m_home_menu.get_attach_widget()) {
                    m_home_menu.attach_to_widget(*this);
                }

                auto count = wnck::get_windows_count();
                m_HomeMinimizeAllWindowsMenuItem.set_sensitive(count);
                m_HomeUnMinimizeAllWindowsMenuItem.set_sensitive(count);
                m_home_menu_close_all_item.set_sensitive(count);

                m_home_menu.popup(sigc::mem_fun(*this, &Panel::on_home_menu_position), 0,
                                  event->time);

                m_preview_open = true;
                return true;
            }

            // handle items menu
            if (m_dockitem_index) {
                auto size = dockitem->get_childmap().size();

                m_item_menu_close_all.set_sensitive(size);
                m_item_menu_minimize_all.set_sensitive(size);
                m_item_menu_unminimize_all.set_sensitive(size);

                // remove old items
                for (auto& itemMenu : m_item_menu.get_children()) {
                    auto type = dynamic_cast<Gtk::ImageMenuItem*>(itemMenu);
                    if (type) {
                        m_item_menu.remove(*itemMenu);
                    }
                }

                if (m_separatorMenuItem) m_item_menu.remove(*m_separatorMenuItem);

                if (size) {
                    m_separatorMenuItem = Gtk::manage(new Gtk::SeparatorMenuItem());
                    m_item_menu.append(*m_separatorMenuItem);

                    // populate childrens;
                    for (auto& item : dockitem->get_childmap()) {
                        auto child = item.second;

                        Gtk::ImageMenuItem* menu_item =
                            Gtk::manage(new Gtk::ImageMenuItem(child->get_title()));

                        Glib::RefPtr<Gdk::Pixbuf> pixbuf = child->get_icon_from_window(16);
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
                }

                m_item_menu.show_all();

                m_item_menu_attach.set_active(dockitem->get_attached());
                m_item_menu.popup_at_pointer(nullptr);
                m_item_menu.popup(sigc::mem_fun(*this, &Panel::on_item_menu_position), 0,
                                  event->time);
            }
        }

        return true;
    }

}  // namespace docklight

