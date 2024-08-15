// clang-format off
#include <giomm/notification.h>
#include <giomm/icon.h>
#include <giomm/themedicon.h>

#include "appwindow.h"
#include "appoptionsgroup.h"
// clang-format on

namespace docklight
{

    Glib::RefPtr<Gtk::Application> AppWindow::m_application;

    AppWindow::AppWindow()
    {
        this->set_title(DOCKLIGHT_APPNAME);

        // m_panel.signal_update().connect(sigc::mem_fun(this, &AppWindow::on_update));

        this->createWindow();
    }

    AppWindow::~AppWindow()
    {
        g_print("Free the AppWindow object.\n");
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

        // A window to implement a docking bar used for creating the dock panel.
        //    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
        this->set_skip_taskbar_hint(true);
        this->set_skip_pager_hint(true);
        //    this->set_keep_above(true);
        //    this->set_decorated(false);
        // wnck_set_client_type(WNCK_CLIENT_TYPE_PAGER);

        // this->set_default_size(1, 1);

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

        //  device::monitor::
    }

    int AppWindow::init(Glib::RefPtr<Gtk::Application>& app)
    {
        app->signal_activate().connect(sigc::ptr_fun(&AppWindow::on_app_activated));
        app->signal_command_line().connect(
            sigc::bind(sigc::ptr_fun(&AppWindow::on_command_line), app), false);

        Gdk::Screen::get_default()->signal_monitors_changed().connect(
            sigc::mem_fun(this, &AppWindow::on_monitor_changed));

        return EXIT_SUCCESS;
    }

    void AppWindow::send_notification(const std::string& title, const std::string& text,
                                      const std::string& icon_name)
    {
        auto Notification = Gio::Notification::create(title);
        Notification->set_body(text);
        auto Icon = Gio::ThemedIcon::create(icon_name);
        Notification->set_icon(Icon);

        m_application->send_notification(Notification);
    }

    void AppWindow::on_app_activated()
    {
        g_print("Applcation activated. Initialize panel.\n");
        AppWindow::send_notification(DOCKLIGHT_APPNAME, "Application start", "dialog-information");
    }

    int AppWindow::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line,
                                   Glib::RefPtr<Gtk::Application>& app)
    {
        int argc = 0;
        char** argv = command_line->get_arguments(argc);
        AppWindow::m_application = app;

        if (argc > 1) {
            Glib::OptionContext ctx;
            AppOptionsGroup group;
            ctx.set_main_group(group);

            try {
                ctx.parse(argc, argv);
            } catch (const Glib::Error& ex) {
                std::cout << "Exception: " << ex.what() << std::endl;
                return EXIT_FAILURE;
            }

            try {
                group.validate();

            } catch (const Glib::Error& ex) {
                std::cout << "Exception: " << ex.what() << std::endl;
                return EXIT_FAILURE;
            };

            std::cout << group.toString() << std::endl;

            for (auto&& t : group.getList()) {
                std::cout << "R:" << std::get<0>(t) << ", " << std::get<1>(t) << ", "
                          << std::get<2>(t) << std::endl;
            }
        }

        std::cout << "\ndisplay detected monitors:" << std::endl;

        for (int i = 0; i < device::monitor::get_monitor_count(); i++) {
            auto const m = device::monitor::get_monitor(i);

            Gdk::Rectangle geometry, workarea;

            m->get_geometry(geometry);
            m->get_workarea(workarea);

            g_print("Monitor# %d %s g: %d x %d  w: %d x %d\n", i, m->get_model().c_str(),
                    geometry.get_width(), geometry.get_height(), workarea.get_width(),
                    workarea.get_height());
        }

        g_print("Default monitor: %d %s\n", device::monitor::get_monitor_number(),
                device::monitor::get_current()->get_model().c_str());

        app->activate();
        return EXIT_SUCCESS;
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
            g_print("Press\n");

            // shared_ptr<DockItem> item = shared_ptr<DockItem>(new DockItem());
            // shared_ptr<DockItem> item = shared_ptr<DockItem>(new DockItem());

            // Glib::RefPtr<DockItem> item = Glib::RefPtr<DockItem>(new DockItem());
            // item->m_image = m_icon;

            // m_panel.add(item);
        }
        //  std::cout << m_display.get_screen_count() << std::endl;

        return true;
    }

    void AppWindow::on_monitor_changed()
    {
        //     device::monitor::set_primary();
        g_warning("!!!!!!!!!!!!!! on_monitor_change.\n");
        update_position();
    }

    void AppWindow::update_position()
    {
        // position_util::set_window_position();
        // g_print("AppWindow updated.\n");

        /*// int area = 64 + 20;  // config::get_dock_area();
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
        this->move(xpos, ypos);*/
    }
}  // namespace docklight
