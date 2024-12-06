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

    void PanelHide::connect_signal_handler(bool connect)
    {
        if (connect) {
            m_sigc_autohide = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &PanelHide::on_autohide), 1000 / 30);
        } else {
            m_sigc_autohide.disconnect();
        }
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
                hide();
            } else {
                unhide();
            }
        }
        //  }
        // g_print("INTER %s\n", intersects ? "yes" : "no");
        return true;
    }

    void PanelHide::hide()
    {
        //
        g_message("PanelHide::Hide");
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
