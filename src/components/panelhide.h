#pragma once

#include <glibmm/main.h>
#include <glibmm/timer.h>
#include <gtkmm/window.h>
#include <sigc++/sigc++.h>

#include "components/dockmenu.h"
#include "components/position.h"
#include "utils/easing.h"
#include "utils/wnck.h"

namespace docklight
{

    class PanelHide  //: Glib::Object
    {
      public:
        PanelHide();
        bool get_lock_render();

        bool on_autohide();

        typedef sigc::signal<void, int, int> type_signal_hide;
        type_signal_hide signal_hide();
        bool m_visible = true;

      private:
        void connect_signal_handler(bool connect);
        void connect_signal_hide(bool connect);
        void connect_signal_unhide(bool connect);
        bool on_hide();
        bool on_unhide();

        static bool is_window_intersect(WnckWindow* window);
        static void on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data);

      private:
        const int m_frame_rate = 60;
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

        int m_area = 0;

        float m_startPosition = 0.f;
        float m_endPosition = 100.f;
        float m_initTime = 0.0f;
        float m_endTime = m_initTime + 10.5;
    };

}  // namespace docklight
