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

    void PanelHide::connect_signal_hide(bool connect)
    {
        if (connect) {
            m_sigc_hide = Glib::signal_timeout().connect(sigc::mem_fun(this, &PanelHide::on_hide),
                                                         1000 / m_frame_rate);
        } else {
            m_sigc_hide.disconnect();
        }
    }

    // void PanelHide::connect_signal_unhide(bool connect)
    //{
    // if (connect) {
    // m_sigc_hide = Glib::signal_timeout().connect(sigc::mem_fun(this, &PanelHide::on_unhide),
    // 1000 / m_frame_rate);
    //} else {
    // m_sigc_hide.disconnect();
    //}
    //}

    void PanelHide::connect_signal_handler(bool connect)
    {
        if (connect) {
            m_sigc_autohide =
                Glib::signal_timeout().connect(sigc::mem_fun(this, &PanelHide::on_autohide), 200);
        } else {
            m_sigc_autohide.disconnect();
        }
    }

    PanelHide::type_signal_hide PanelHide::signal_hide()
    {
        return m_signal_hide;
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

        bool intersects = is_window_intersect(m_active_window);
        bool fullscreeen = wnck_window_is_fullscreen(m_active_window);

        if (m_last_intersects != intersects) {
            m_last_intersects = intersects;

            Position()->window_intersects(intersects);

            if (intersects) {  // hide
                if (!m_visible) return true;
                if (Config()->is_autohide_none() && !fullscreeen) {
                    return true;
                }

                m_startPosition = 0.f;
                m_endPosition = (float)m_area;

                m_initTime = 0.f;
                m_endTime = 10.0f;

                m_animation_time = 0;
                connect_signal_hide(true);
            } else {  // show

                if (fullscreeen) return true;
                if (m_visible) return true;

                m_startPosition = (float)m_area;
                m_endPosition = 0.f;

                m_initTime = 0.f;
                m_endTime = 5.0f;

                m_animation_time = 0;
                connect_signal_hide(true);
            }
        }

        return true;
    }

    bool PanelHide::on_hide()
    {
        m_offset_x = 0;
        m_offset_y = 0;
        float position = easing::map_clamp(m_animation_time, m_initTime, m_endTime, m_startPosition,
                                           m_endPosition, &easing::linear::easeOut);

        get_offset(position, m_offset_x, m_offset_y);

        m_signal_hide.emit(m_offset_x, m_offset_y);
        m_animation_time++;

        if (m_visible) {
            if ((int)position >= m_area) {
                connect_signal_hide(false);
                m_visible = false;
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

    void PanelHide::force_visible()
    {
        if (m_visible) return;

        m_startPosition = (float)m_area;
        m_endPosition = 0.f;

        m_initTime = 0.f;
        m_endTime = 5.0f;

        m_animation_time = 0;
        connect_signal_hide(true);
    }

    bool PanelHide::get_visible()
    {
        return m_visible;
    }

}  // namespace docklight
