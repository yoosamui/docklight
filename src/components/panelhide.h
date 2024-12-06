#pragma once

#include <glibmm/main.h>
#include <gtkmm/window.h>
#include <sigc++/sigc++.h>

#include "components/position.h"
#include "utils/wnck.h"
namespace docklight
{

    class PanelHide : public Glib::Object
    {
      public:
        PanelHide();
        bool get_lock_render();

      private:
        void connect_signal_handler(bool connect);
        void hide();
        void unhide();

        bool on_autohide();

        static bool is_window_intersect(WnckWindow* window);
        static void on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data);

      private:
        sigc::connection m_sigc_autohide;
        static WnckWindow* m_active_window;

        bool m_lock_render = false;
        bool m_last_intersects = false;
    };

}  // namespace docklight
