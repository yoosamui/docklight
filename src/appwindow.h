#pragma once

//#include <gdkmm/screen.h>
#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include "common.h"

DL_NS_BEGIN

class appwindow : public Gtk::Window
{
  public:
    appwindow();
    ~appwindow();
    int init(Glib::RefPtr<Gtk::Application> &app);

  private:
    static int on_command_line(
        const Glib::RefPtr<Gio::ApplicationCommandLine> &command_line,
        Glib::RefPtr<Gtk::Application> &app);
};

DL_NS_END
