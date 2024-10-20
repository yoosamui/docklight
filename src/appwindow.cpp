// clang-format off
#include <giomm/notification.h>
#include <giomm/icon.h>
#include <giomm/themedicon.h>
#include <glibmm/i18n.h>

#include "appwindow.h"
#include "appoptionsgroup.h"
#include "components/position.h"
#include "translations.h"

// clang-format on

namespace docklight
{

    // static members declaration
    Panel* AppWindow::m_panel;
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

        Config();
        m_observer = create_observer();
        m_position = create_position(this);

        m_panel = new Panel();

        this->add(*m_panel);
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

        Config()->load();
        m_panel->init(app);

        return EXIT_SUCCESS;
    }

    void AppWindow::send_notification(const Glib::ustring& title, const Glib::ustring& text,
                                      const Glib::ustring& icon_name)
    {
        auto Notification = Gio::Notification::create(title);
        Notification->set_body(text);
        auto Icon = Gio::ThemedIcon::create(icon_name);
        Notification->set_icon(Icon);

        // TODO: just for testing
        m_application->send_notification(Notification);
    }

    void AppWindow::on_app_activated()
    {
        g_message(MSG_APPLICATION_ACTIVATED);
        AppWindow::send_notification(DOCKLIGHT_APPNAME, MSG_APPLICATION_START,
                                     "dialog-information");
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
        return false;
        ;
        if ((event->type == GDK_BUTTON_PRESS)) {
            //  this->update_position();
            g_print("\n");
            g_print("--->Window Press\n\n");
            g_print("\n");

            return false;
            int factor = Config()->get_icon_size();
            g_print("----------------------------------ara - %d\n", factor);
            //  factor -= 1;

            Config()->set_icon_size(factor);
            //  factor = Config()->get_dock_area();
            g_print("------------------------------factor-%d---icon - %d\n", factor,
                    Config()->get_icon_size());
            m_position->reset_position();

            // m_panel->container_updated();

            // Gtk::Widget::queue_draw();
            //             m_position->set_struts();

            // position::struts::set_strut(false);
            // wnck::get_docks();
            return false;

            //            m_composite = Glib::RefPtr<ExplodesWindow>(new ExplodesWindow());
            m_composite.show_at(800, 800);
            auto container = Provider();
            for (auto& dockitem : container->data()) {
                const auto map = dockitem->get_childmap();
                g_print("%lu [%s] %lu\n", dockitem->get_xid(), dockitem->get_group_name().c_str(),
                        map.size());

                for (auto it = map.begin(); it != map.end(); it++) {
                    std::shared_ptr<DockItemIcon> dockitem = it->second;

                    g_print("***%lu [%s]\n", dockitem->get_xid(),
                            dockitem->get_group_name().c_str());
                }
            }
            /*for (auto it = m_appmap.begin(); it != m_appmap.end(); it++) {
            // g_print("Owner %d %s\n", dockitem->to_string().c_str());
            //               g_print("Owner %d %s\n", dockitem->get_xid(),
            //               dockitem->get_group_name().c_str());
            // for (const auto& it : dockitem->get_childmap()) {
            // const Glib::RefPtr<DockItem> dockitem = item.second;
            // g_print("    Child %d %s\n", dockitem->get_xid(),
            // dockitem->get_group_name().c_str());
            //}

            //    Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();
            auto m_appmap = container->get_map();
            for (auto it = m_appmap.begin(); it != m_appmap.end(); it++) {
                //  for (const auto& item : container->get_appmap()) {
                const Glib::RefPtr<DockItemIcon> dockitem = it->second;

                auto pixbuf = dockitem->get_icon();

                char filepath[512];
                sprintf(filepath, "/home/yoo/TEMP/%s-(%s) [%lu] {%d}",
                        dockitem->get_group_name().c_str(),
        dockitem->get_instance_name().c_str(), dockitem->get_xid(),
        (int)dockitem->get_childmap().size());

                if (pixbuf) {
                    pixbuf->save(filepath, "png");
                }
                g_print("%s\n", dockitem->to_string().c_str());

                for (const auto& it : dockitem->get_childmap()) {
                    const Glib::RefPtr<DockItemIcon> dockitem = it.second;

                    std::string title(dockitem->get_window_name());
                    std::replace(title.begin(), title.end(), ' ', '-');

                    sprintf(filepath, "/home/yoo/TEMP/  *****%s-(%s) [%lu]",
                            dockitem->get_group_name().c_str(),
                            dockitem->get_instance_name().c_str(), dockitem->get_xid());

                    // g_print("*****%s-(%s)%s{%d}\n", dockitem->get_icon_name().c_str(),
                    // dockitem->get_title().c_str(), dockitem->get_window_name().c_str(),
                    // dockitem->get_xid());

                    g_print("*****%s\n", dockitem->to_string().c_str());

                    pixbuf = dockitem->get_icon();
                    if (pixbuf) {
                        pixbuf->save(filepath, "png");
                    }
                }
            }

        }*/
        }
        return true;
    }

    void AppWindow::on_monitor_changed()
    {
        g_message("on_monitor_changed.");
        m_position->on_monitor_changed();
    }

    // bool AppWindow::on_enter_notify_event(GdkEventCrossing* crossing_event)
    //{
    //// m_mouse_in = true;
    // return true;
    //}

    // bool AppWindow::on_leave_notify_event(GdkEventCrossing* crossing_event)
    //{
    ////   m_mouse_in = false;
    // return true;
    //}
    void AppWindow::update_position()
    {
        // m_panel->container_updated();

        // position_util::set_window_position();
        // g_print("AppWindow updated.\n");

        /*// int area = 64 + 20;  // config::get_dock_area();
        int area = config::get_dock_area();
        auto const workarea = get_workarea();
        //  auto const alignment = config::get_dock_alignment();
        // auto const panel = m_window->get_panel();
        int xpos = 0, ypos = 0, center = 0;

        int width = m_panel.get_required_size();
        center = workarea.get_width() / 2 - width / 2;
        xpos = workarea.get_x() + center;

        ypos = workarea.get_y() + workarea.get_height() - area;

        this->resize(width, area);
        this->move(xpos, ypos);*/
    }

}  // namespace docklight
