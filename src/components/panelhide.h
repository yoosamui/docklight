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

    class PanelHide  // : Glib::Object
    {
        typedef sigc::signal<void, int, int> type_signal_hide;

      public:
        PanelHide();

        void force_visible();

        bool on_autohide();
        bool get_visible();

        type_signal_hide signal_hide();

      private:
        void get_offset(float position, int& offset_x, int& offset_y);

        void connect_signal_handler(bool connect);
        void connect_signal_hide(bool connect);

        bool on_hide();

        static bool is_window_intersect(WnckWindow* window);
        static void on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data);

      private:
        static WnckWindow* m_active_window;

        const int m_frame_rate = 60;

        bool m_visible = true;
        bool m_last_intersects = false;

        type_signal_hide m_signal_hide;
        sigc::connection m_sigc_hide;
        sigc::connection m_sigc_autohide;

        int m_offset_x = 0;
        int m_offset_y = 0;
        int m_animation_time = 0;
        int m_area = 0;

        float m_startPosition = 0.f;
        float m_endPosition = 0.f;
        float m_initTime = 0.0f;
        float m_endTime = 0.0;
    };

}  // namespace docklight
