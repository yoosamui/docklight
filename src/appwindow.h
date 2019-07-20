#pragma once

#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include "common.h"
#include "components/panel.h"
DL_NS_BEGIN

class AppWindow : public Gtk::Window
{
  public:
    AppWindow();
    ~AppWindow();
    int init(Glib::RefPtr<Gtk::Application> &app);
    static void update();
    Panel *get_panel();

  private:
    Panel m_panel;
    static int on_command_line(
        const Glib::RefPtr<Gio::ApplicationCommandLine> &command_line,
        Glib::RefPtr<Gtk::Application> &app);
    static void monitor_changed_callback(GdkScreen *screen, gpointer gtkwindow);
};

DL_NS_END
