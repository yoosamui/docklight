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
// clang-format on

DL_NS_BEGIN

class AppWindow : public Gtk::Window
{
  public:
    AppWindow();
    ~AppWindow();
    int init(Glib::RefPtr<Gtk::Application> &app);

    static void update();

    Panel *get_panel();

    Gdk::Gravity m_gravity;

  private:
    Panel m_panel;
    bool m_mouse_in = false;
    // bool m_animation_running = false;
    // bool m_animation_hidde = false;
    // float m_animation_time = 0;
    // int m_animation_state = 1;

    // Glib::Timer m_animation_timer;

    static int on_command_line(
        const Glib::RefPtr<Gio::ApplicationCommandLine> &command_line,
        Glib::RefPtr<Gtk::Application> &app);
    static void monitor_changed_callback(GdkScreen *screen, gpointer gtkwindow);

    bool on_enter_notify_event(GdkEventCrossing *crossing_event);
    bool on_leave_notify_event(GdkEventCrossing *crossing_event);
    //    bool auto_hidde_timer();
};

DL_NS_END
