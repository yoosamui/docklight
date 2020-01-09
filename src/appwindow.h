#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

// clang-format off
#include <gtkmm/main.h>
#include <glibmm/timer.h>
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <libwnck/libwnck.h>
#include <sigc++/sigc++.h>
#include "common.h"
#include "components/panel.h"
#include <gdkmm/monitor.h>
// clang-format on

DL_NS_BEGIN

class AppWindow : public Gtk::Window
{
  public:
    AppWindow();
    ~AppWindow();
    int init(Glib::RefPtr<Gtk::Application> &app);

    static void update();

    Panel *get_panel() const;
    static void send_message(const string &title, const string &text, const string &icon_name);

  private:
    static Glib::RefPtr<Gtk::Application> m_application;

    static Panel *m_panel;

    bool m_mouse_in = false;

    static void on_app_activated();

    static int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine> &command_line,
                               Glib::RefPtr<Gtk::Application> &app);
    static void monitor_changed_callback(GdkScreen *screen, gpointer gtkwindow);
    static void monitor_size_changed_callback(GdkScreen *screen, gpointer gtkwindow);

    void on_monitors_changed();
    void on_monitor_added(const Glib::RefPtr<Gdk::Monitor> &monitor);

    bool on_enter_notify_event(GdkEventCrossing *crossing_event);
    bool on_leave_notify_event(GdkEventCrossing *crossing_event);
    //    bool auto_hidde_timer();
};

DL_NS_END
