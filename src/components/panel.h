#pragma once

// clang-format off
#include <glibmm/main.h>
#include <gtkmm/drawingarea.h>
#include <glibmm/timer.h>
#include <math.h>

#include "components/config.h"
#include "utils/easing.h" // for testing
//#include <cmath>
//#include <ctime>
#include "dockitemprovider.h"
#include "components/TransparentWindow.h"
#include "components/position.h"
#include "components/dockrender.h"
// clang-format on

namespace docklight
{

    class Panel : public DockRender
    {
      public:
        Panel();
        ~Panel();
        void init();
        void container_updated(guint explicit_size = 0);
        guint m_icon_size = 0;
        //  int m_icon_size = 0;

        int get_scalling_factor();
        int get_scalling_factor_down();

      private:
        sigc::connection m_sigc_draw;
        sigc::connection m_sigc_updated;

        guint get_dockitem_index(int mx, int my);

        bool on_button_press_event(GdkEventButton* event);
        bool on_motion_notify_event(GdkEventMotion* event);
        void on_container_updated(window_action_t action, int index);

        bool on_timeout_draw();

        bool m_mouse_enter = false;
        bool on_enter_notify_event(GdkEventCrossing* crossing_event) override;
        bool on_leave_notify_event(GdkEventCrossing* crossing_event) override;

        Glib::Timer m_animation_timer;

      private:
        Glib::RefPtr<DockItemProvider> m_provider;
        gint m_dockitem_index = -1;
        easing::bounce m_bounce;
    };
}  // namespace docklight

