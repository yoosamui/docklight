#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1
//#include <gtkmm.h>

//#include <giomm/notification.h>
#include <gtkmm/application.h>
//#include <gtkmm/box.h>
#include <gtkmm/label.h>
//#include <gtkmm/drawingarea.h>
//#include <gtkmm/enums.h>
//#include <gtkmm/frame.h>
#include <glibmm/main.h>
#include <gtkmm/window.h>
#include <libwnck/libwnck.h>
#include <sigc++/sigc++.h>

#include "common.h"
#include "components/panel.h"

DL_NS_BEGIN

class AppWindow : public Gtk::Window
{
    Panel m_panel;

  protected:
    void on_monitors_changed();
    void on_monitor_added(const Glib::RefPtr<Gdk::Monitor>& monitor);
    bool on_timeout_draw();
    bool on_button_press_event(GdkEventButton* event) override;

    static void send_Notify(const string& title, const string& text, const string& icon_name);
    static int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line,
                               Glib::RefPtr<Gtk::Application>& app);

    void createWindow();

  public:
    AppWindow();
    int init(Glib::RefPtr<Gtk::Application>& app);
    void update_position();
};

DL_NS_END
