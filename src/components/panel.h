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
#include "utils/wnck.h"
#include "components/dockitemicon.h"
#include "dockitemprovider.h"
#include "components/TransparentWindow.h"
#include "components/position.h"
#include "components/dockrender.h"
//#include "components/dockmenu.h"
// clang-format on
#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/menuitem.h>
namespace docklight
{

    class Panel : public DockRender
    {
      public:
        Panel();
        ~Panel();

        void init(Glib::RefPtr<Gtk::Application> app);
        void container_updated(guint explicit_size = 0);

        inline guint get_scale_factor()
        {
            // remember the bigest area
            static int area = 0;
            if (!area) area = Config()->get_dock_area() + Config()->get_separator_size();

            const int max_icon_size = Config()->get_custom_icon_size();
            const auto workarea = device::monitor::get_workarea();
            const int num_items = m_provider->data().size();
            const int item_width = area;

            int screen_width = 0;
            int reduce_screen_value = Config()->get_dock_area_margin() * num_items;

            if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                if (workarea.get_width() <= 1) return max_icon_size;
                screen_width = workarea.get_width() - reduce_screen_value;
            } else {
                if (workarea.get_height() <= 1) return max_icon_size;
                screen_width = workarea.get_height() - reduce_screen_value;
            }

            // Calculate the scaling factor
            float scaling_factor =
                static_cast<float>(screen_width) / static_cast<float>(num_items * item_width);

            int icon_size = std::floor(max_icon_size * scaling_factor);
            if (icon_size > max_icon_size) {
                icon_size = max_icon_size;
            }

            return icon_size;
        }

      private:
        Glib::RefPtr<Gtk::Application> m_app;
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
        // Mouse handlers
        // bool on_button_press_event(GdkEventButton* event);
        // bool on_button_release_event(GdkEventButton* event);
        // bool on_motion_notify_event(GdkEventMotion* event);
        bool on_scroll_event(GdkEventScroll* e) override;

        // Menus
        void on_item_menu_childlist_event(WnckWindow* window);

        void on_home_menu_quit_event() override;
        void on_home_menu_position(int& x, int& y, bool& push_in) override
        {
            DockMenu::on_home_menu_position(x, y, push_in);
        }

        void on_item_menu_position(int& x, int& y, bool& push_in) override
        {
            DockMenu::on_item_menu_position(x, y, push_in);
        }

      private:
        Glib::RefPtr<DockItemProvider> m_provider;
        easing::bounce m_bounce;
    };
}  // namespace docklight

