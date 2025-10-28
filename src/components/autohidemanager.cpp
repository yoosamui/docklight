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

#include "components/autohidemanager.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace docklight
{

    Glib::RefPtr<AutohideManager> m_autohide;

    Glib::RefPtr<AutohideManager> create_autohide()
    {
        if (!m_autohide) {
            m_autohide = Glib::RefPtr<AutohideManager>(new AutohideManager());
        }

        return m_autohide;
    }

    Glib::RefPtr<AutohideManager> Autohide()
    {
        g_assert(m_autohide);
        return m_autohide;
    }

    WnckWindow* AutohideManager::m_active_window;
    AutohideManager::AutohideManager()
    {
        m_area = Config()->get_dock_area();
        m_active_window = nullptr;

        WnckScreen* wnckscreen = wnck::get_default_screen();
        g_signal_connect(wnckscreen, "active_window_changed",
                         G_CALLBACK(AutohideManager::on_active_window_changed), nullptr);

        connect_signal_handler(true);
        if (Config()->is_autohide()) m_autohide_timer.start();
    }

    AutohideManager::~AutohideManager()
    {
        connect_signal_handler(false);
        m_sigc_hide.disconnect();
        m_sigc_autohide.disconnect();
    }

    void AutohideManager::connect_signal_show(bool connect)
    {
        if (connect) {
            m_sigc_show = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &AutohideManager::on_show), 1000 / 60);
        } else {
            m_sigc_show.disconnect();
        }
    }
    void AutohideManager::connect_signal_hide(bool connect)
    {
        if (connect) {
            m_sigc_hide = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &AutohideManager::on_hide), 1000 / 60);
        } else {
            m_sigc_hide.disconnect();
        }
    }

    void AutohideManager::connect_signal_handler(bool connect)
    {
        if (connect) {
            m_sigc_autohide = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &AutohideManager::on_autohide), 1000 / 1);
        } else {
            m_sigc_autohide.disconnect();
        }
    }

    AutohideManager::type_signal_hide AutohideManager::signal_hide()
    {
        return m_signal_hide;
    }

    AutohideManager::type_signal_before_hide AutohideManager::signal_before_hide()
    {
        return m_signal_before_hide;
    }

    AutohideManager::type_signal_after_hide AutohideManager::signal_after_hide()
    {
        return m_signal_after_hide;
    }

    void AutohideManager::on_active_window_changed(WnckScreen* screen,
                                                   WnckWindow* previously_active_window,
                                                   gpointer user_data)
    {
        // Gets the active WnckWindow on screen.
        // May return NULL sometimes, since not all
        // window managers guarantee that a window is always active.
        m_active_window = wnck_screen_get_active_window(screen);
    }

    bool AutohideManager::is_window_intersect(WnckWindow* window)
    {
        if (!window) return false;

        WnckWindowType wt = wnck_window_get_window_type(window);
        if (wt == WNCK_WINDOW_DESKTOP) return false;

        Gdk::Rectangle win_rect = wnck::get_window_geometry(window);
        Gdk::Rectangle dock_rect = Position()->get_window_geometry();

        return dock_rect.intersects(win_rect);
    }

    void AutohideManager::get_offset(float position, int& offset_x, int& offset_y)
    {
        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            if (Config()->get_dock_location() == dock_location_t::top) {
                offset_x = 0;
                offset_y = -(int)position;
            } else {
                offset_x = 0;
                offset_y = (int)position;
            }
        } else {
            if (Config()->get_dock_location() == dock_location_t::left) {
                offset_x = -(int)position;
                offset_y = 0;
            } else {
                offset_x = (int)position;
                offset_y = 0;
            }
        }
    }

    bool AutohideManager::on_autohide()
    {
        if (!m_active_window) return true;

        WnckWindowType wt = wnck_window_get_window_type(m_active_window);
        if (wt == WNCK_WINDOW_DESKTOP) return true;

        m_area = Config()->get_dock_area();

        m_intersects = is_window_intersect(m_active_window);
        m_fullscreen = wnck_window_is_fullscreen(m_active_window) && m_intersects;

        // none
        if (Config()->is_autohide_none() && m_fullscreen) {
            hide_now();
            return true;
        }

        if (!m_visible && Config()->is_autohide_none() && !m_fullscreen) {
            show_now();
            return true;
        }

        // autohide
        if (m_visible && Config()->is_autohide()) {
            if (m_fullscreen) {
                m_autohide_timer.stop();
                hide_now();
                return true;
            }

            if (m_autohide_timer.elapsed() > 2) {
                m_autohide_timer.stop();
                hide_now();
                return true;
            }

            return true;
        }

        // intelihide
        if (Config()->is_intelihide()) {
            if (m_fullscreen) {
                hide_now();
                return true;
            }

            if (m_last_intersects != m_intersects) {
                m_last_intersects = m_intersects;

                if (m_intersects) {
                    hide_now();
                    return true;
                } else {
                    show_now();
                    return true;
                }
            }

            if (!m_visible && !m_intersects) {
                show_now();
                return true;
            }
        }

        return true;
    }

    bool AutohideManager::on_show()
    {
        m_offset_x = 0;
        m_offset_y = 0;

        auto m_start_position = (float)m_area;
        auto m_end_position = 0.f;

        auto m_init_time = 0.f;
        auto m_end_time = m_show_delay;

        float position =
            easing::map_clamp(m_animation_time, m_init_time, m_end_time, m_start_position,
                              m_end_position, &easing::linear::easeInOut);

        // if ((int)position == 0) {
        // g_print("Reset\n");
        // connect_signal_show(false);
        // return true;
        //}

        get_offset(position, m_offset_x, m_offset_y);

        // g_message("SHOW %2d) TIME:%f %f - POS  %f %f Y:%d %f area:%d", m_animation_time,
        // position * m_init_time, m_end_time, m_start_position, m_end_position, m_offset_y,
        // position, m_area);

        m_signal_hide.emit(m_offset_x, m_offset_y);
        m_animation_time++;

        if ((int)position < 1) {
            Position()->set_window_passthrought(false);
            connect_signal_show(false);
            m_visible = true;

            return false;
        }

        /*if (m_visible) {
            // if ((int)position >= m_area) {
            if ((int)position >= m_area) {
                connect_signal_hide(false);
                m_visible = false;

                m_signal_after_hide.emit(0);
                return true;
            }

        } else {
            // if ((int)position < 1) {
            if ((int)position < 1) {
                connect_signal_hide(false);
                m_visible = true;

                return true;
            }
        }*/

        //        connect_signal_hide(false);
        return true;
    }
    bool AutohideManager::on_hide()
    {
        m_offset_x = 0;
        m_offset_y = 0;

        auto m_start_position = 0.f;
        auto m_end_position = (float)m_area;

        auto m_init_time = 0.f;
        auto m_end_time = m_hide_delay;

        float position =
            easing::map_clamp(m_animation_time, m_init_time, m_end_time, m_start_position,
                              m_end_position, &easing::linear::easeInOut);

        get_offset(position, m_offset_x, m_offset_y);

        // g_message("HIDE %2d) TIME:%f %f - POS  %f %f Y:%d %f area:%d", m_animation_time,
        // position * m_init_time, m_end_time, m_start_position, m_end_position, m_offset_y,
        // position, m_area);

        m_signal_hide.emit(m_offset_x, m_offset_y);
        m_animation_time++;

        if ((int)position >= m_area) {
            connect_signal_hide(false);
            m_visible = false;

            m_signal_after_hide.emit(0);

            Position()->set_window_passthrought(true);
            return false;
        }

        /*if (m_visible) {
            // if ((int)position >= m_area) {
            if ((int)position >= m_area) {
                connect_signal_hide(false);
                m_visible = false;

                m_signal_after_hide.emit(0);
                return true;
            }

        } else {
            // if ((int)position < 1) {
            if ((int)position < 1) {
                connect_signal_hide(false);
                m_visible = true;

                return true;
            }
        }*/

        //        connect_signal_hide(false);
        return true;
    }

    void AutohideManager::show_now()
    {
        if (m_visible) return;

        //  const std::lock_guard<std::mutex> lock(m_mutex);

        m_start_position = (float)m_area;
        m_end_position = 0.f;

        m_init_time = 0.f;
        m_end_time = m_show_delay;

        m_animation_time = 0;

        // m_frame_time = g_get_monotonic_time();
        connect_signal_show(true);
    }

    void AutohideManager::hide_now()
    {
        if (!m_visible) return;

        // const std::lock_guard<std::mutex> lock(m_mutex);

        m_start_position = 0.f;
        m_end_position = (float)m_area;

        m_init_time = 0.f;
        m_end_time = m_hide_delay;

        m_animation_time = 0;
        m_signal_before_hide.emit(0);
        //_frame_time = g_get_real_time();
        connect_signal_hide(true);
    }

    void AutohideManager::force_show()
    {
        show_now();
    }

    bool AutohideManager::get_visible() const
    {
        return m_visible;
    }

    bool AutohideManager::get_fullscreen() const
    {
        return m_fullscreen;
    }

    bool AutohideManager::get_intersects() const
    {
        return m_intersects;
    }

    bool AutohideManager::get_autohide_allow() const
    {
        return m_hide_allow;
    }

    void AutohideManager::set_hide_allow(bool allow)
    {
        if (allow == m_hide_allow) return;

        m_autohide_timer.reset();
        m_autohide_timer.stop();

        if (allow) m_autohide_timer.start();

        m_hide_allow = allow;
    }
}  // namespace docklight
