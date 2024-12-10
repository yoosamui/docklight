// clang-format off
#include <giomm/notification.h>
#include <giomm/icon.h>
#include <giomm/themedicon.h>
#include <glibmm/i18n.h>

#include "appwindow.h"
#include "appoptionsgroup.h"
#include "components/position.h"
#include "translations.h"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

// clang-format on

namespace docklight
{

    Glib::RefPtr<Gtk::Application> AppWindow::m_application;

    AppWindow::AppWindow()
    {
#define DOCK_WINDOW 1
#ifdef DOCK_WINDOW
        // A window to implement a docking bar used for creating the dock panel.
        set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
        set_decorated(false);
#else
        set_decorated(true);

#endif
        set_resizable(true);
        set_skip_taskbar_hint(true);
        set_skip_pager_hint(true);
        set_keep_above(true);

        set_size_request(1, 1);

        m_config = create_config();
        m_provider = create_provider();
        m_observer = create_observer();
        m_position = create_position(this);
        m_autohide = create_autohide();

        m_panel = new Panel();

        add(*m_panel);
        show_all();

        g_message("Create AppWindow.");
    }

    AppWindow::~AppWindow()
    {
        g_message(MSG_FREE_OBJECT, "AppWindow");
    }

    int AppWindow::init(Glib::RefPtr<Gtk::Application>& app)
    {
        app->signal_activate().connect(sigc::ptr_fun(&AppWindow::on_app_activated));

        app->signal_command_line().connect(
            sigc::bind(sigc::ptr_fun(&AppWindow::on_command_line), app), false);

        Gdk::Screen::get_default()->signal_monitors_changed().connect(
            sigc::mem_fun(this, &AppWindow::on_monitor_changed));

        // m_sigc_updated = get_dockitem_provider()->signal_update().connect(
        // sigc::mem_fun(this, &AppWindow::on_container_updated));

        m_config->load();
        m_panel->init(app, m_config, m_provider, m_position);

        return EXIT_SUCCESS;
    }

    /*void AppWindow::send_notification(const Glib::ustring& title, const Glib::ustring& text,
                                      const Glib::ustring& icon_name)
    {
        [>auto Notification = Gio::Notification::create(title);
        Notification->set_body(text);
        auto Icon = Gio::ThemedIcon::create(icon_name);
        Notification->set_icon(Icon);

        // TODO: just for testing
        m_application->send_notification(Notification);<]
    }*/

    void AppWindow::on_app_activated()
    {
        g_message(MSG_APPLICATION_ACTIVATED);
        // AppWindow::send_notification(DOCKLIGHT_APPNAME, MSG_APPLICATION_START,
        //"dialog-information");
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
                group.validate();
            } catch (const Glib::Error& ex) {
                g_warning("on_command_line:Exception: %s", ex.what().c_str());
            }

            std::cout << group.toString() << std::endl;
            auto const args_list = group.getList();

            // add args to config
            Config()->set_arguments(args_list);
        }

        g_message("Window Manager : %s", system::get_window_manager_name().c_str());
        g_message("Is Composite : %s", system::is_composite() ? "yes" : "no");
        g_message("Is Mutter WM: %s", system::is_mutter_window_manager() ? "yes" : "No");
        g_message("DL Version: %s", PACKAGE_VERSION);

        // if (system::is_composite()) {
        // system::redirecting_all_toplevel_windows_to_offscreen_pixmaps();
        // g_message("Redirect toplevel_windows");
        //}

        device::monitor::set_current_monitor(Config()->get_monitor_name());

        std::cout << "\n" << MSG_DISPLAY_DETECTED_MONITORS << " :" << std::endl;

        for (int i = 0; i < device::monitor::get_monitor_count(); i++) {
            auto const m = device::monitor::get_monitor(i);

            Gdk::Rectangle geometry, workarea;

            m->get_geometry(geometry);
            m->get_workarea(workarea);

            // clang-format off
            std::cout << MSG_MONITOR  << " " <<  MSG_MODEL  << ":" << m->get_model() << std::endl
                      << MSG_GEOMETRY << " " <<  MSG_WIDTH  << "=" << geometry.get_width() << std::endl
                      << MSG_GEOMETRY << " " <<  MSG_HEIGHT << "=" << geometry.get_height() << std::endl
                      << MSG_WORKAREA << " " <<  MSG_WIDTH  << "=" << workarea.get_width() << std::endl
                      << MSG_WORKAREA << " " <<  MSG_HEIGHT << "=" << workarea.get_height() << std::endl
                      << std::endl;

            // clang-format on
        }

        std::cout << MSG_DEFAULT_MONITOR << ": " << device::monitor::get_monitor_number() << ", "
                  << device::monitor::get_current()->get_model() << std::endl;

        app->activate();
        return EXIT_SUCCESS;
    }

    bool AppWindow::on_button_press_event(GdkEventButton* event)
    {
        return true;
    }

    void AppWindow::on_monitor_changed()
    {
        g_message("on_monitor_changed.");
        m_position->on_monitor_changed();
    }

}  // namespace docklight
