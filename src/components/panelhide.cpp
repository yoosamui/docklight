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

#include "components/panelhide.h"

namespace docklight
{

    WnckWindow* PanelHide::m_active_window;
    PanelHide::PanelHide()
    {
        m_area = Config()->get_dock_area() / 2;
        m_active_window = nullptr;

        WnckScreen* wnckscreen = wnck::get_default_screen();
        g_signal_connect(wnckscreen, "active_window_changed",
                         G_CALLBACK(PanelHide::on_active_window_changed), nullptr);

        connect_signal_handler(true);
    }

    PanelHide::~PanelHide()
    {
        m_sigc_hide.disconnect();
        m_sigc_autohide.disconnect();
    }

    void PanelHide::connect_signal_hide(bool connect)
    {
        if (connect) {
            m_sigc_hide = Glib::signal_timeout().connect(sigc::mem_fun(this, &PanelHide::on_hide),
                                                         1000 / m_frame_rate);
        } else {
            m_sigc_hide.disconnect();
        }
    }

    void PanelHide::connect_signal_handler(bool connect)
    {
        if (connect) {
            m_sigc_autohide = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &PanelHide::on_autohide), 1000 / 3);
        } else {
            m_sigc_autohide.disconnect();
        }
    }

    PanelHide::type_signal_hide PanelHide::signal_hide()
    {
        return m_signal_hide;
    }

    PanelHide::type_signal_before_hide PanelHide::signal_before_hide()
    {
        return m_signal_before_hide;
    }

    PanelHide::type_signal_after_hide PanelHide::signal_after_hide()
    {
        return m_signal_after_hide;
    }

    void PanelHide::on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data)
    {
        // Gets the active WnckWindow on screen.
        // May return NULL sometimes, since not all
        // window managers guarantee that a window is always active.
        m_active_window = wnck_screen_get_active_window(screen);
    }

    bool PanelHide::is_window_intersect(WnckWindow* window)
    {
        if (!window) return false;

        WnckWindowType wt = wnck_window_get_window_type(window);
        if (wt == WNCK_WINDOW_DESKTOP) return false;

        Gdk::Rectangle win_rect = wnck::get_window_geometry(window);
        Gdk::Rectangle dock_rect = Position()->get_window_geometry();

        return dock_rect.intersects(win_rect);
    }

    void PanelHide::get_offset(float position, int& offset_x, int& offset_y)
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
                //
            } else {
                offset_x = (int)position;
                offset_y = 0;
            }
        }
    }

    bool PanelHide::on_autohide()
    {
        if (!m_active_window) return true;

        m_intersects = is_window_intersect(m_active_window);
        // bool fullscreeen = wnck_window_is_fullscreen(m_active_window);

        if (m_last_intersects != m_intersects) {
            m_last_intersects = m_intersects;

            Position()->window_intersects(m_intersects);

            if (m_intersects) {
                hide_now();
            } else {
                show_now();
            }
        }

        return true;
    }

    bool PanelHide::on_hide()
    {
        m_offset_x = 0;
        m_offset_y = 0;
        float position =
            easing::map_clamp(m_animation_time, m_init_time, m_end_time, m_start_position,
                              m_end_position, &easing::linear::easeOut);

        get_offset(position, m_offset_x, m_offset_y);

        m_signal_hide.emit(m_offset_x, m_offset_y);
        m_animation_time++;

        if (m_visible) {
            if ((int)position >= m_area) {
                connect_signal_hide(false);
                m_visible = false;
                m_signal_after_hide.emit(0);
                return true;
            }

        } else {
            if ((int)position <= 0) {
                connect_signal_hide(false);
                m_visible = true;
                return true;
            }
        }

        return true;
    }

    void PanelHide::show_now()
    {
        bool fullscreeen = wnck_window_is_fullscreen(m_active_window);
        if (fullscreeen) return;
        if (m_visible) return;

        m_start_position = (float)m_area;
        m_end_position = 0.f;

        m_init_time = 0.f;
        m_end_time = m_show_delay;

        m_animation_time = 0;
        connect_signal_hide(true);

        //   m_last_intersects = false;

        // if (m_visible) return;
        // if (wnck_window_is_fullscreen(m_active_window)) return;
        //  m_start_position = (float)m_area;
        //  m_end_position = 0.f;

        // m_init_time = 0.f;
        // m_end_time = m_show_delay;

        // m_animation_time = 0;
        // connect_signal_hide(true);
    }

    void PanelHide::hide_now()
    {
        bool fullscreeen = wnck_window_is_fullscreen(m_active_window);
        if (!m_visible) return;
        if (Config()->is_autohide_none() && !fullscreeen) {
            return;
        }

        m_start_position = 0.f;
        m_end_position = (float)m_area;

        m_init_time = 0.f;
        m_end_time = m_hide_delay;

        m_animation_time = 0;
        m_signal_before_hide.emit(0);
        connect_signal_hide(true);

        // if (!m_visible) return;
        // if (wnck_window_is_fullscreen(m_active_window)) return;

        // m_start_position = 0.f;
        // m_end_position = (float)m_area;

        // m_init_time = 0.f;
        // m_end_time = m_hide_delay;

        // m_animation_time = 0;
        // connect_signal_hide(true);
    }

    void PanelHide::force_show()
    {
        show_now();
    }

    bool PanelHide::get_visible() const
    {
        return m_visible;
    }

    bool PanelHide::get_intersects() const
    {
        return m_intersects;
    }

}  // namespace docklight
