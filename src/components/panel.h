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

        int get_scalling_factor(int& carea)
        {
            static int area = 0;
            if (!area) area = Config()->get_dock_area() + Config()->get_separator_size();
            carea = area;

            const auto workarea = device::monitor::get_workarea();
            const int num_items = m_provider->data().size();
            const int item_width = area;

            int screen_width = workarea.get_width() - area;
            if (screen_width <= 1) return 128;

            if (Position()->get_width() > workarea.get_width()) {
                // screen_width -= area;  // Position()->get_width();
                //  screen_width Position()->get_width();
            }
            //  screen_width = Position()->get_width();
            if (screen_width <= 1) return 128;
            /// screen_width = Position()->get_width();
            // Position()->get_width();  //- (area * 2);
            // screen_width -= area;

            // Calculate the scaling factor
            float scaling_factor =
                static_cast<float>(screen_width) / static_cast<float>(num_items * item_width);

            int icon_size = std::floor(128 * scaling_factor);
            if (icon_size > 128) {
                g_print("ICON_SIZE SCALES RAW %d\n", icon_size);

                icon_size = 128;  // std::abs(icon_size - 128);
            }

            g_print(
                "XXX SCALE %f  screen_width %d workaarea_width: %d size: %ld  area: %d icon_zize: "
                "%d\n",
                scaling_factor, screen_width, workarea.get_width(), m_provider->data().size(), area,
                icon_size);

            //            g_print("%d\n", icon_size);
            return icon_size;
        }
        int compute_scalling_factor(const guint size, int& carea);
        int get_scalling_factorX(int& carea);
        int get_scalling_factor_down(int& carea);
        //        int get_scalling_factor_down();

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

