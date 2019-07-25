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

    // this->set_gravity(Gdk::Gravity::GRAVITY_STATIC);

    // A window to implement a docking bar used for creating the dock panel.
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_keep_above(true);
    this->set_decorated(false);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

    this->add(m_panel);
    this->show_all();
    m_gravity = Gdk::GRAVITY_NORTH_WEST;
    // m_animation_timer.start();
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

    // Glib::signal_timeout().connect(
    // sigc::mem_fun(*this, &AppWindow::auto_hidde_timer), 1000 / 30);
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
    g_print("mousein\n");
    m_mouse_in = true;
    return true;
}

bool AppWindow::on_leave_notify_event(GdkEventCrossing *crossing_event)
{
    m_mouse_in = false;
    return true;
}
/*float m_easing_duration = 16.0;
float m_initTime = 0;
bool m_visible = true;
bool AppWindow::auto_hidde_timer()
{
    // if (m_animation_state == 0) {
    //  return false;
    //}

    if (m_visible && !m_mouse_in && !m_animation_running &&
        abs(m_animation_timer.elapsed()) > 1.5f) {
        g_print("hidde\n");
        this->set_gravity(Gdk::GRAVITY_NORTH_EAST);
        m_animation_running = true;
        m_animation_hidde = true;
        //  getchar();
    }

    if (!m_visible && m_mouse_in && !m_animation_running) {
        g_print("show\n");
        // this->set_gravity(m_gravity);
        this->set_gravity(Gdk::GRAVITY_NORTH_EAST);
        m_animation_running = true;
    }

    if (m_animation_running) {
        auto endTime = m_initTime + m_easing_duration;

        float startPosition;
        float endPosition;

        if (m_visible) {
            startPosition = config::get_dock_area();
            endPosition = 1.0;
        } else {
            startPosition = 1.0;
            endPosition = config::get_dock_area();
        }

        float resize_pos = easing_util::map_clamp(
            m_animation_time, m_initTime, endTime, startPosition, endPosition,
            &easing_util::linear::easeIn);

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            this->resize(this->get_width(), resize_pos);
        } else {
            this->resize(resize_pos, this->get_height());
        }

        g_print("anim %d\n", (int)resize_pos);
        m_animation_time++;

        if ((int)resize_pos == (int)endPosition) {
            m_animation_time = 0.f;
            m_animation_timer.stop();
            m_animation_timer.reset();

            m_animation_timer.start();
            m_animation_running = false;

            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                m_visible = this->get_height() > 4;
                g_print("END %d %d\n", (int)m_visible, this->get_height());
            } else {
                m_visible = this->get_width() > 4;
                g_print("END %d %d\n", (int)m_visible, this->get_width());
            }

            [>this->set_gravity(Gdk::Gravity::GRAVITY_NORTH_WEST);

            if (!m_visible) {
                int y, x, width;
                this->get_position(x, y);

                resize(1, get_height());
                width = resize_pos;

                move(2559, y);
            }<]
        }
    }

    return true;
}*/

void AppWindow::update()
{
    position_util::set_window_position();
    g_print("AppWindow update \n");
}

int AppWindow::on_command_line(
    const Glib::RefPtr<Gio::ApplicationCommandLine> &command_line,
    Glib::RefPtr<Gtk::Application> &app)
{
    g_print("ACTIVATE\n");
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
