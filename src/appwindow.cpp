// clang-format off
#include "appwindow.h"
#include <getopt.h>
#include <glibmm/main.h>
#include "components/arguments.h"
#include "components/config.h"
#include "components/device.h"
#include "utils/position.h"
#include "utils/system.h"

#include <giomm/notification.h>
#include <giomm/icon.h>
#include <giomm/themedicon.h>
// clang-format on

DL_NS_BEGIN

Panel *AppWindow::m_panel;
Glib::RefPtr<Gtk::Application> AppWindow::m_application;

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

    //    this->set_gravity(Gdk::Gravity::GRAVITY_STATIC);
    this->set_size_request(1, 1);

    // A window to implement a docking bar used for creating the dock panel.
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_keep_above(true);
    this->set_decorated(false);

    wnck_set_client_type(WNCK_CLIENT_TYPE_PAGER);
    m_panel = new Panel();
    this->add(*m_panel);

    m_panel->set_owner(this);
    this->show_all();
}

AppWindow::~AppWindow()
{
    delete m_panel;
    g_print("free AppWindow class.\n");
}

int AppWindow::init(Glib::RefPtr<Gtk::Application> &app)
{
    position_util::init(this);

    app->signal_command_line().connect(sigc::bind(sigc::ptr_fun(&AppWindow::on_command_line), app),
                                       false);

    // The monitors-changed signal is emitted when the number,
    // size or position of the monitors attached to the screen change.
    /*auto const screen = device::display::get_default_screen()->gobj();
    g_signal_connect(G_OBJECT(screen), "monitors-changed", G_CALLBACK(monitor_changed_callback),
                     (gpointer)this);

    g_signal_connect(G_OBJECT(screen), "size-changed", G_CALLBACK(monitor_size_changed_callback),
                     (gpointer)this);*/

    // The signal_monitors_changed() signal is emitted when the number, size or position of the
    // monitors attached to the screen change.
    Gdk::Screen::get_default()->signal_monitors_changed().connect(
        sigc::mem_fun(this, &AppWindow::on_monitors_changed));

    Gdk::Display::get_default()->signal_monitor_added().connect(
        sigc::mem_fun(this, &AppWindow::on_monitor_added));

    app->signal_activate().connect(sigc::ptr_fun(&AppWindow::on_app_activated));

    return 0;
}

void AppWindow::on_app_activated()
{
    g_print("Applcation activated. Initialize panel.\n");
    m_panel->init();
}

// The signal_monitors_changed() signal is emitted when the number, size or position of the
// monitors attached to the screen change.
static int m_monitor_number = 0;

void AppWindow::on_monitor_added(const Glib::RefPtr<Gdk::Monitor> &monitor)
{
    //
    //  g_print("Monitors add %d %d\n", m_monitor_number, device::monitor::get_monitor_count());
    //  device::monitor::set_current_monitor(m_monitor_number);

    device::monitor::set_primary();
    update();
}

void AppWindow::on_monitors_changed()
{
    // g_print("Monitors changes %d %d\n", m_monitor_number, device::monitor::get_monitor_count());
    // device::monitor::set_current_monitor(m_monitor_number);
    //// auto const monitor = device::monitor::get_current();

    // g_print("Default monitor: %d %s\n", m_monitor_number,
    // device::monitor::get_current()->get_model().c_str());

    device::monitor::set_primary();
    update();
}

void AppWindow::monitor_size_changed_callback(GdkScreen *screen, gpointer gtkwindow) {}

void AppWindow::monitor_changed_callback(GdkScreen *screen, gpointer gtkwindow) {}

Panel *AppWindow::get_panel() const
{
    return m_panel;
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
    g_print("AppWindow updated.\n");
}

void AppWindow::send_message(const string &title, const string &text, const string &icon_name)
{
    auto Notification = Gio::Notification::create(title);
    Notification->set_body(text);
    auto Icon = Gio::ThemedIcon::create(icon_name);
    Notification->set_icon(Icon);

    m_application->send_notification(Notification);
}

int AppWindow::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine> &command_line,
                               Glib::RefPtr<Gtk::Application> &app)
{
    int argc = 0;
    char **argv = command_line->get_arguments(argc);

    AppWindow::m_application = app;

    cli::Arguments a(argc, argv);
    if (a.validate() == false) {
        app->quit();
    }

    const GSList *list = a.get_results();

    if (!list) {
        g_print("No parameters provided.\n");
    }

    // lOAD config
    config::load(list);
    g_print("load config done.\n");

    // display the detected monitors
    for (int i = 0; i < device::monitor::get_monitor_count(); i++) {
        auto const m = device::monitor::get_monitor(i);

        Gdk::Rectangle geometry, workarea;

        m->get_geometry(geometry);
        m->get_workarea(workarea);

        g_print("Monitor# %d %s g: %d x %d  w: %d x %d\n", i, m->get_model().c_str(),
                geometry.get_width(), geometry.get_height(), workarea.get_width(),
                workarea.get_height());
    }

    m_monitor_number = device::monitor::get_monitor_number();
    g_print("Default monitor: %d %s\n", m_monitor_number,
            device::monitor::get_current()->get_model().c_str());

    g_print("window manager: %s\n", system_util::get_window_manager_name().c_str());

    // activate window
    app->activate();

    send_message("Docklight", "Application start", "dialog-information");
    return argc;
}

DL_NS_END
