#include <getopt.h>

#include <gdkmm/rectangle.h>
#include "appwindow.h"
#include "common.h"
#include "components/arguments.h"
#include "components/config.h"
#include "components/device.h"
#include "utils/pixbuf.h"

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

    //// Set event masks
    // add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
    // Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK |
    // Gdk::POINTER_MOTION_HINT_MASK | Gdk::FOCUS_CHANGE_MASK |
    // Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK |
    // Gdk::POINTER_MOTION_HINT_MASK | Gdk::POINTER_MOTION_MASK);
    this->set_gravity(Gdk::Gravity::GRAVITY_STATIC);

    // A window to implement a docking bar used for creating the dock panel.
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_keep_above(true);

    // this->set_decorated(false);
    // this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

    this->show_all();
}

AppWindow::~AppWindow()
{
    g_print("free AppWindow class.\n");
}

int AppWindow::init(Glib::RefPtr<Gtk::Application> &app)
{
    app->signal_command_line().connect(
        sigc::bind(sigc::ptr_fun(&AppWindow::on_command_line), app), false);

    return 0;
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

    for (const GSList *l = list; l; l = l->next) {
        cli::result_t *data = static_cast<cli::result_t *>(l->data);
        g_print("arguments: --%c %d %s\n", data->arg, data->int_value,
                data->string_value);
    }

    // Load config
    config::load(list);

    // display the detected monitors
    for (int i = 0; i < device::monitor::get_monitor_count(); i++) {
        auto const m = device::monitor::get_monitor(i);

        Gdk::Rectangle geometry, workarea;

        m->get_geometry(geometry);
        m->get_workarea(workarea);

        g_print("Monitor# %d %s g:%d x %d  w: %d x %d\n", i,
                m->property_model().get_value().c_str(), geometry.get_width(),
                geometry.get_height(), workarea.get_width(),
                workarea.get_height());
    }

    // activate window
    app->activate();

    return argc;
}

DL_NS_END
