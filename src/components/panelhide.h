#pragma once

#include <glibmm/main.h>
#include <glibmm/timer.h>
#include <gtkmm/window.h>
#include <sigc++/sigc++.h>

#include "components/dockmenu.h"
#include "components/position.h"
#include "utils/easing.h"
#include "utils/wnck.h"

// class DockRender : public Gtk::DrawingArea
namespace docklight
{

    class PanelHide : Glib::Object
    {
        typedef sigc::signal<void, int, int> type_signal_hide;
        type_signal_hide signal_hide();

      public:
        PanelHide();
        bool get_lock_render();

      private:
        void connect_signal_handler(bool connect);
        void connect_signal_hide(bool connect);
        bool on_hide();
        void unhide();

        bool on_autohide();

        static bool is_window_intersect(WnckWindow* window);
        static void on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data);

      private:
        type_signal_hide m_signal_hide;

        sigc::connection m_sigc_hide;
        sigc::connection m_sigc_autohide;
        static WnckWindow* m_active_window;

        bool m_lock_render = false;
        bool m_last_intersects = false;

        int m_animation_time = 0;
        Glib::Timer m_animation_timer;

        int m_offset_x = 0;
        int m_offset_y = 0;

        float m_startPosition = 0.f;
        float m_endPosition = 100.f;
        float m_initTime = 0.0f;
        float m_endTime = m_initTime + 10.5;
    };

}  // namespace docklight
