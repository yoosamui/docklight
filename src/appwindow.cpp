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
        m_animation = create_animation_manager();

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

        GdkScreen* screen = gdk_screen_get_default();
        auto is_composite = gdk_screen_is_composited(screen);

        /*GdkRGBA* background_color = NULL;
        PangoFontDescription* font_desc = NULL;
        gint border_radius = 0;
        GtkStyleContext* style_context = nullptr;
        // auto ww = Widget::get_style_context();

        gtk_style_context_get_style(style_context);
        if (style_context) {
            gtk_style_context_get(style_context, gtk_style_context_get_state(style_context),
                                  GTK_STYLE_PROPERTY_BACKGROUND_COLOR, &background_color, NULL,
                                  NULL, NULL, NULL, NULL);
            gtk_style_context_get(style_context, gtk_style_context_get_state(style_context),
                                  GTK_STYLE_PROPERTY_BACKGROUND_COLOR, &background_color,
                                  GTK_STYLE_PROPERTY_FONT, &font_desc,
                                  GTK_STYLE_PROPERTY_BORDER_RADIUS, &border_radius, NULL);

            g_print("%f\n", background_color->red);

            if (background_color != NULL) gdk_rgba_free(background_color);
            if (font_desc != NULL) pango_font_description_free(font_desc);
        }
*/
        // https://docs.gtk.org/gtk3/method.StyleContext.get.html
        GtkSettings* settings;
        gchar* theme_name;
        settings = gtk_settings_get_default();
        g_object_get(settings, "gtk-theme-name", &theme_name, NULL);

        // Get default settings
        //  GtkSettings* settings = gtk_settings_get_default();

        // Create a sample window
        GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        //  gtk_window_set_title(GTK_WINDOW(window), "Background Color Example");
        //  gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

        // Get style context for the window
        GtkStyleContext* context = gtk_widget_get_style_context(window);

        // Get background color
        GdkRGBA* bcolor = NULL;
        GdkRGBA* fcolor = NULL;

        gtk_style_context_get(context, gtk_style_context_get_state(context),
                              GTK_STYLE_PROPERTY_BACKGROUND_COLOR, &bcolor, NULL, NULL, NULL, NULL,
                              NULL);

        gtk_style_context_get(context, gtk_style_context_get_state(context),
                              GTK_STYLE_PROPERTY_COLOR, &fcolor, NULL, NULL, NULL, NULL, NULL);

        //        gtk_style_context_get_background_color(context, GTK_STATE_FLAG_SELECTED, &bcolor);

        // gtk_style_context_get_background_color(context, GTK_STATE_FLAG_NORMAL, &color);
        g_message("bcolor: %f,%f,%f %f", bcolor->red, bcolor->green, bcolor->blue, bcolor->alpha);
        g_message("fcolor: %f,%f,%f %f", fcolor->red, fcolor->green, fcolor->blue, fcolor->alpha);

        if (bcolor) gdk_rgba_free(bcolor);
        if (fcolor) gdk_rgba_free(fcolor);

        // Print color values
        // std::cout << "Background color: RGBA(" << static_cast<double>(color.red) << ", "
        //<< static_cast<double>(color.green) << ", " << static_cast<double>(color.blue)
        //<< ", " << static_cast<double>(color.alpha) << ")" << std::endl;

        g_message("Window Manager : %s", system::get_window_manager_name().c_str());
        g_message("Theme name : %s", theme_name);
        g_message("Is Composite : %s", is_composite ? "yes" : "no");
        g_message("Is Mutter WM: %s", system::is_mutter_window_manager() ? "yes" : "No");
        g_message("DL Version: %s", PACKAGE_VERSION);

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

    void AppWindow::set_window_passthrought(bool passthrough)
    {
#ifdef GDK_WINDOWING_X11
        auto gdk_window = get_window();
        if (!gdk_window) return;

        Display* dpy = GDK_WINDOW_XDISPLAY(gdk_window->gobj());
        ::Window xid = GDK_WINDOW_XID(gdk_window->gobj());

        int w = get_width();
        int h = get_height();

        const int OFFSET = 10;

        // Create a region of the Full window that receives input
        Region region = XCreateRegion();
        XRectangle full{0, 0, static_cast<unsigned short>(w), static_cast<unsigned short>(h)};
        XUnionRectWithRegion(&full, region, region);

        Region hole_region = XCreateRegion();
        XRectangle hole;

        if (passthrough) {
            //  g_message("Passthrough ON");

            // Subtract rectangle click-through
            if (Config()->get_dock_location() == dock_location_t::bottom) {
                hole = XRectangle{0, 0, static_cast<unsigned short>(w),
                                  static_cast<unsigned short>(h - OFFSET)};

            } else if (Config()->get_dock_location() == dock_location_t::top) {
                hole = XRectangle{0, OFFSET, static_cast<unsigned short>(w),
                                  static_cast<unsigned short>(h - OFFSET)};

            } else if (Config()->get_dock_location() == dock_location_t::left) {
                hole = XRectangle{OFFSET, 0, static_cast<unsigned short>(w - OFFSET),
                                  static_cast<unsigned short>(h - OFFSET)};

            } else if (Config()->get_dock_location() == dock_location_t::right) {
                hole = XRectangle{0, 0, static_cast<unsigned short>(w - OFFSET),
                                  static_cast<unsigned short>(h)};
            }

            XUnionRectWithRegion(&hole, hole_region, hole_region);
            XSubtractRegion(region, hole_region, region);
            XDestroyRegion(hole_region);

        } else {
            // g_message("passthrough OFF");

            // Full window receives input → region covers whole window
            XRectangle full{0, 0, static_cast<unsigned short>(w), static_cast<unsigned short>(h)};
            XUnionRectWithRegion(&full, region, region);
        }

        // Apply region as input mask
        XShapeCombineRegion(dpy, xid, ShapeInput, 0, 0, region, ShapeSet);
        XDestroyRegion(region);
        XFlush(dpy);

#endif
    }
}  // namespace docklight
