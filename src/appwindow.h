
//#include <gtkmm.h>

#include <giomm/notification.h>
#include <gtkmm/application.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/enums.h>
#include <gtkmm/frame.h>
#include <gtkmm/window.h>
#include <sigc++/sigc++.h>

#include "common.h"

DL_NS_BEGIN

class AppWindow : public Gtk::Window
{
  protected:
    void on_monitors_changed();
    void on_monitor_added(const Glib::RefPtr<Gdk::Monitor>& monitor);

  public:
    AppWindow();
    int init(Glib::RefPtr<Gtk::Application>& app);
    void update_position();
};

DL_NS_END
