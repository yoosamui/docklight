#include "appwindow.h"
#include <getopt.h>
#include <glibmm/main.h>
// clang-format off
//#include <gdkmm/rectangle.h>
//#include <glibmm/timer.h>
//#include <sigc++/sigc++.h>
//#include <gtkmm/main.h>
//#include <glibmm-2.4/glibmm/timer.h>
//#include <glibmm/timer.h>
//#include "common.h"
#include "components/arguments.h"
#include "components/config.h"
#include "components/device.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/easing.h"
// clang-format on
DL_NS_BEGIN

AppWindow::AppWindow()
{
    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    // A window to implement a docking bar used for creating the dock panel.
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_keep_above(true);
    this->set_decorated(false);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

    this->add(m_panel);
    this->show_all();
}

AppWindow::~AppWindow()
{
    g_print("free AppWindow class.\n");
}

int AppWindow::init(Glib::RefPtr<Gtk::Application> &app)
{
    position_util::init(this);
    app->signal_command_line().connect(
        sigc::bind(sigc::ptr_fun(&AppWindow::on_command_line), app), false);

    // The monitors-changed signal is emitted when the number,
    // size or position of the monitors attached to the screen change.
    auto const screen = device::display::get_default_screen()->gobj();
    g_signal_connect(G_OBJECT(screen), "monitors-changed",
                     G_CALLBACK(monitor_changed_callback), (gpointer)this);

    return 0;
}

void AppWindow::monitor_changed_callback(GdkScreen *screen, gpointer gtkwindow)
{
    update();
}

Panel *AppWindow::get_panel()
{
    return &m_panel;
}

bool AppWindow::on_enter_notify_event(GdkEventCrossing *crossing_event)
{
    m_mouse_in = true;
    return true;
}

bool AppWindow::on_leave_notify_event(GdkEventCrossing *crossing_event)
{
    m_mouse_in = false;
    return true;
}
void AppWindow::update()
{
    position_util::set_window_position();
    g_print("AppWindow update \n");
}

int AppWindow::on_command_line(
    const Glib::RefPtr<Gio::ApplicationCommandLine> &command_line,
    Glib::RefPtr<Gtk::Application> &app)
{
    int argc = 0;
    char **argv = command_line->get_arguments(argc);

    cli::Arguments a(argc, argv);
    if (a.validate() == false) {
        return -1;
    }

    const GSList *list = a.get_results();

    if (!list) {
        g_print("No parameters provided.\n");
    }

    // Load config
    config::load(list);

    // display the detected monitors
    for (int i = 0; i < device::monitor::get_monitor_count(); i++) {
        auto const m = device::monitor::get_monitor(i);

        Gdk::Rectangle geometry, workarea;

        m->get_geometry(geometry);
        m->get_workarea(workarea);

        g_print("Monitor# %d %s g: %d x %d  w: %d x %d\n", i,
                m->get_model().c_str(), geometry.get_width(),
                geometry.get_height(), workarea.get_width(),
                workarea.get_height());
    }

    g_print(
        "Default monitor: %d %s\n",
        device::monitor::get_current()->get_monitor_number(),
        device::monitor::get_current()->get_monitor_obj()->get_model().c_str());

    // resize and set the position
    AppWindow::update();

    // activate window
    app->activate();

    return argc;
}

DL_NS_END
