#include "components/panelhide.h"

namespace docklight
{

    WnckWindow* PanelHide::m_active_window;
    PanelHide::PanelHide()
    {
        //
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

    void PanelHide::connect_signal_unhide(bool connect)
    {
        if (connect) {
            m_sigc_hide = Glib::signal_timeout().connect(sigc::mem_fun(this, &PanelHide::on_unhide),
                                                         1000 / m_frame_rate);
        } else {
            m_sigc_hide.disconnect();
        }
    }

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

        bool intersects = is_window_intersect(m_active_window);
        bool fullscreeen = wnck_window_is_fullscreen(m_active_window);

        m_lock_render = intersects;

        //  if (intersects) {
        if (m_last_intersects != intersects) {
            m_last_intersects = intersects;
            //   g_message("PanelHide::INTER %s", intersects ? "yes" : "no");
            Position()->window_intersects(intersects);
            //   Provider()->emit_update();

            if (intersects) {
                if (Config()->is_autohide_none() && !fullscreeen) {
                    return true;
                }

                m_startPosition = 0.f;
                m_endPosition = (float)m_area;

                m_initTime = 0.f;
                m_endTime = 7.0f;

                g_print("HIDE START\n");
                m_animation_time = 0;
                connect_signal_hide(true);
                //  m_update_required = true;
                //    m_visible = true;
            } else {
                if (m_visible) return true;

                m_startPosition = (float)m_area;
                m_endPosition = 0.f;

                m_initTime = 0.f;
                m_endTime = 5.0f;
                g_print("UNHIDE START\n");
                m_animation_time = 0;
                connect_signal_unhide(true);
                //  m_visible = false;
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

        m_signal_hide.emit(m_offset_x, m_offset_y);
        m_animation_time++;
        //    g_print(">>%d HIDE %d x %d\n", m_animation_time, m_offset_x, m_offset_y);

        if ((int)position >= m_area) {
            m_sigc_hide.disconnect();

            g_print("HIDE END\n");
            m_visible = false;
            return true;
        }

        return true;
    }

    bool PanelHide::on_unhide()
    {
        float position = easing::map_clamp(m_animation_time, m_initTime, m_endTime, m_startPosition,
                                           m_endPosition, &easing::linear::easeIn);

        // {std::make_pair(Function::Bounce, Type::InOut), bounce::easeInOut},
        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            m_offset_x = 0;
            m_offset_y = (int)position;
        } else {
            m_offset_x = (int)position;
            m_offset_y = 0;
        }

        m_signal_hide.emit(m_offset_x, m_offset_y);
        m_animation_time++;
        //  g_print("<< UNHIDE %d x %d\n", m_offset_x, m_offset_y);

        //  / if (m_animation_time > 12)

        if ((int)position <= 0) {
            m_sigc_hide.disconnect();

            g_print("UNHIDE END!\n");
            m_visible = true;
            return true;
        }

        return true;
        // g_message("PanelHide::unhide");
    }

    bool PanelHide::get_lock_render()
    {
        return m_lock_render;
    }
}  // namespace docklight
