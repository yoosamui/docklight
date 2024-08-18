#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

// clang-format off
#include <gtkmm/application.h>
#include <glibmm/main.h>
#include <gtkmm/window.h>

#include "constants.h"
#include "components/panel.h"
#include "components/device.h"
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
//#include <gtkmm/enums.h>
//#include <gtkmm/frame.h>

namespace docklight
{

    class AppWindow : public Gtk::Window
    {
      private:
        Panel m_panel;
        static Glib::RefPtr<Gtk::Application> m_application;

        void createWindow();

        bool on_timeout_draw();
        bool on_button_press_event(GdkEventButton* event) override;

        // Monitor related event
        void on_monitor_changed();
        void update_position();

        static void send_Notify(const Glib::ustring& title, const Glib::ustring& text,
                                const Glib::ustring& icon_name);
        static int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line,
                                   Glib::RefPtr<Gtk::Application>& app);

        static void on_app_activated();
        static void send_notification(const Glib::ustring& title, const Glib::ustring& text,
                                      const Glib::ustring& icon_name);

      public:
        AppWindow();
        ~AppWindow();

        int init(Glib::RefPtr<Gtk::Application>& app);
    };

}  // namespace docklight
