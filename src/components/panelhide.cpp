#include "components/panelhide.h"

namespace docklight
{

    WnckWindow* PanelHide::m_active_window;
    PanelHide::PanelHide()
    {
        //

        m_active_window = nullptr;

        WnckScreen* wnckscreen = wnck::get_default_screen();
        g_signal_connect(wnckscreen, "active_window_changed",
                         G_CALLBACK(PanelHide::on_active_window_changed), nullptr);

        connect_signal_handler(true);
    }

    void PanelHide::connect_signal_hide(bool connect)
    {
        /*if (connect) {
            m_sigc_hide =
                Glib::signal_timeout().connect(sigc::mem_fun(this, &PanelHide::on_hide), 1000 / 20);
        } else {
            m_sigc_hide.disconnect();
        }*/
    }

    void PanelHide::connect_signal_handler(bool connect)
    {
        /*if (connect) {
            m_sigc_autohide = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &PanelHide::on_autohide), 1000 / 2);
        } else {
            m_sigc_autohide.disconnect();
        }*/
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

        if (!m_active_window) {
            return;
        }
    }

    bool PanelHide::is_window_intersect(WnckWindow* window)
    {
        g_assert(window);

        Gdk::Rectangle win_rect = wnck::get_window_geometry(window);
        Gdk::Rectangle dock_rect = Position()->get_window_geometry();

        return dock_rect.intersects(win_rect);
    }

    bool PanelHide::on_autohide()
    {
        if (!m_active_window) return true;

        bool fullscreen = wnck_window_is_fullscreen(m_active_window);
        bool intersects = is_window_intersect(m_active_window);

        m_lock_render = intersects;
        //  if (intersects) {
        if (m_last_intersects != intersects) {
            m_last_intersects = intersects;
            //   g_message("PanelHide::INTER %s", intersects ? "yes" : "no");
            Position()->window_intersects(intersects);
            Provider()->emit_update();

            if (intersects) {
                m_animation_timer.start();

                m_startPosition = 0.f;
                m_endPosition = 100.f;
                m_initTime = 0.0f;
                m_endTime = m_initTime + 10.5;

                m_animation_time = 0;
                connect_signal_hide(true);
            } else {
                unhide();
                m_animation_timer.start();
            }
        }
        //  }
        // g_print("INTER %s\n", intersects ? "yes" : "no");
        return true;
    }

    bool PanelHide::on_hide()
    {
        float position = easing::map_clamp(m_animation_time, m_initTime, m_endTime, m_startPosition,
                                           m_endPosition, &easing::linear::easeOut);

        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            m_offset_x = 0;
            m_offset_y = (int)position;
        } else {
            m_offset_x = (int)position;
            m_offset_y = 0;
        }

        // m_signal_hide.emit(m_offset_x, m_offset_y);
        m_animation_time++;

        g_message("PanelHide::Hide %f", position);

        if (m_animation_time > 10) {
            m_sigc_hide.disconnect();

            return true;
        }

        return true;
    }

    void PanelHide::unhide()
    {
        g_message("PanelHide::unhide");
    }

    bool PanelHide::get_lock_render()
    {
        return m_lock_render;
    }
}  // namespace docklight
