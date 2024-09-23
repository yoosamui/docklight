#pragma once

// clang-format off
#include <gtkmm/application.h>
#include <glibmm/main.h>
#include <gtkmm/window.h>

#include "constants.h"
#include "components/panel.h"
#include "components/device.h"
#include "components/config.h"



#include "components/dockitemcontainer.h"
#include "components/TransparentWindow.h"
#include "components/ExplodesWindow.h"

// clang-format on

//#include <gtkmm.h>

//#include <giomm/icon.h>
//#include <giomm/notification.h>
//#include <giomm/themedicon.h>
//#include <giomm/notification.h>
//#include <libwnck/libwnck.h>
//#include <sigc++/sigc++.h>

//#include "components/config.h"
//#include "components/device.h"

//##include "utils/position.h"
//#include "utils/system.h"

//#include <gtkmm/box.h>
//#include <gtkmm/label.h>
//#include <gtkmm/drawingarea.h>
//#include <gtkmm/enums.hExplodesWindowude <gtkmm/frame.h>

namespace docklight
{

    // class AppWindow : public Gtk::Window  // ublic TransparentWindow
    class AppWindow : public TransparentWindow
    {
      public:
        AppWindow();
        ~AppWindow();

        int init(Glib::RefPtr<Gtk::Application>& app);
        // Config* m_config = Config::getInstance();
      private:
        //    bool on_timeout_draw();
        bool on_button_press_event(GdkEventButton* event) override;
        // bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

        //  void createWindow();
        void on_monitor_changed();
        static void update_position();

        static void send_Notify(const Glib::ustring& title, const Glib::ustring& text,
                                const Glib::ustring& icon_name);
        static int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line,
                                   Glib::RefPtr<Gtk::Application>& app);

        //  void on_container_updated(window_action_t action, gint index);
        static void on_app_activated();
        static void send_notification(const Glib::ustring& title, const Glib::ustring& text,
                                      const Glib::ustring& icon_name);

        // bool on_enter_notify_event(GdkEventCrossing* crossing_event);
        // bool on_leave_notify_event(GdkEventCrossing* crossing_event);

      private:
        // sigc::connection m_sigc_updated;
        Glib::RefPtr<ExplodesWindow> m_composite;
        // ExplodesWindow m_composite;
        Panel m_panel;
        static Glib::RefPtr<Gtk::Application> m_application;
    };

}  // namespace docklight
