#include "appwindow.h"

#include "appoptionsgroup.h"

//#include <glibmm/optioncontext.h>

DL_NS_BEGIN

AppWindow::AppWindow()
{
    this->set_title("Docklight 5");

    // m_panel.signal_update().connect(sigc::mem_fun(this, &AppWindow::on_update));

    Gdk::Screen::get_default()->signal_monitors_changed().connect(
        sigc::mem_fun(this, &AppWindow::on_monitors_changed));

    Gdk::Display::get_default()->signal_monitor_added().connect(
        sigc::mem_fun(this, &AppWindow::on_monitor_added));

    this->createWindow();
}

int AppWindow::init(Glib::RefPtr<Gtk::Application>& app)
{
    app->signal_command_line().connect(sigc::bind(sigc::ptr_fun(&AppWindow::on_command_line), app),
                                       false);

    //   app->activate();
    return 0;  //
}

void AppWindow::send_Notify(const string& title, const string& text, const string& icon_name)
{
    /*auto Notification = Gio::Notification::create(title);
    auto Icon = Gio::ThemedIcon::create(icon_name);

    Notification->set_body(text);
    Notification->set_icon(Icon);

    m_application->send_notification(Notification);*/
}

int AppWindow::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line,
                               Glib::RefPtr<Gtk::Application>& app)
{
    int argc = 0;
    char** argv = command_line->get_arguments(argc);

    // Set up the current locale.
    setlocale(LC_ALL, "");
    Glib::OptionContext ctx;

    AppOptionsGroup group;
    ctx.set_main_group(group);
    try {
        ctx.parse(argc, argv);
    } catch (const Glib::Error& ex) {
        std::cout << "Exception: " << ex.what() << std::endl;
    }

    std::cout << "parsed values: " << std::endl << "  foo = " << group.m_arg_foo << std::endl;
    //          << "  filename = " << group.m_arg_filename << std::endl
    //           << "  activate_something = " << (group.m_arg_boolean ? "enabled" : "disabled")
    // << std::endl;
    //         << "  goo = " << group.m_arg_goo << std::endl
    //        << "  x-string = " << group.m_arg_x_string << std::endl
    //       << "  x-filename = " << group.m_arg_x_filename << std::endl;

    /*static GOptionEntry entries[] = {
        {"repeats", 'r', 0, G_OPTION_ARG_INT, &repeats, "Average over N repetitions", "N"},
        {"max-size", 'm', 0, G_OPTION_ARG_INT, &max_size, "Test up to 2^M items", "M"},
        {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL},
        {"beep", 'b', 0, G_OPTION_ARG_NONE, &beep, "Beep when done", NULL},
        {"rand", 0, 0, G_OPTION_ARG_NONE, &randomize, "Randomize the data", NULL},
        G_OPTION_ENTRY_NULL};


    // parse arguments:
    Glib::OptionContext ctx;
    Glib::OptionGroup group("options", "main options");

    char** argv = command_line->get_arguments(argc);*/

    // TODO: Handle args

    // display the detected monitors
    /*for (int i = 0; i < device::monitor::get_monitor_count(); i++) {
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
*/
    app->activate();
    return 0;
}

void AppWindow::createWindow()
{
    GdkScreen* screen;
    GdkVisual* visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != nullptr && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    // this->set_default_size(10, 10);

    // this->set_gravity(Gdk::Gravity::GRAVITY_STATIC);
    // this->set_size_request(10, 10);

    // A window to implement a docking bar used for creating the dock panel.
    //    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
    // this->set_skip_taskbar_hint(true);
    // this->set_skip_pager_hint(true);
    // this->set_keep_above(true);
    // this->set_decorated(false);
    // wnck_handle_new(WNCK_CLIENT_TYPE_PAGER);

    Glib::signal_timeout().connect(
        sigc::mem_fun(static_cast<AppWindow*>(this), &AppWindow::on_timeout_draw), 100);

    this->add(m_panel);
    this->show_all();
    // this->show_all_children();
}

bool AppWindow::on_timeout_draw()
{
    //    if (isset) return false;
    //   isset = true;
    //    this->update_position();
    return true;
}

bool AppWindow::on_button_press_event(GdkEventButton* event)
{
    if ((event->type == GDK_BUTTON_PRESS)) {
        //  this->update_position();
        g_print("Press");

        // shared_ptr<DockItem> item = shared_ptr<DockItem>(new DockItem());
        // shared_ptr<DockItem> item = shared_ptr<DockItem>(new DockItem());

        // Glib::RefPtr<DockItem> item = Glib::RefPtr<DockItem>(new DockItem());
        // item->m_image = m_icon;

        // m_panel.add(item);
    }

    return true;
}
void AppWindow::on_monitors_changed()
{
    //
    this->update_position();
}

void AppWindow::on_monitor_added(const Glib::RefPtr<Gdk::Monitor>& monitor)
{
    //
    this->update_position();
}

void AppWindow::update_position()
{
    /*

        // int area = 64 + 20;  // config::get_dock_area();
        int area = config::get_dock_area();
        auto const workarea = get_workarea();
        //  auto const alignment = config::get_dock_alignment();
        // auto const panel = m_window->get_panel();
        int xpos = 0, ypos = 0, center = 0;

        int width = m_panel.get_required_size();
        //   g_print("---------- %d\n", width);
        center = workarea.get_width() / 2 - width / 2;
        xpos = workarea.get_x() + center;

        ypos = workarea.get_y() + workarea.get_height() - area;

        this->resize(width, area);
        this->move(xpos, ypos);
        */
}
DL_NS_END
