#include "appupdater.h"
// test remove after
#include "gio/gdesktopappinfo.h"
namespace docklight
{
#define TESTING
#ifdef TESTING
    bool get_window_icon(WnckWindow* window, Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
    {
        if (!wnck_window_has_name(window)) return false;
        if (wnck_window_get_icon_is_fallback(window)) return false;

        // if (!window_icon_name) return false;

        // Gets the icon to be used for window. If no icon was found, a fallback icon is
        // used. wnck_window_get_icon_is_fallback() can be used to tell if the icon is the
        // fallback icon.
        GdkPixbuf* gdkpixbuf = wnck_window_get_icon(window);

        if (!gdkpixbuf) {
            if (G_IS_OBJECT(gdkpixbuf)) g_object_unref(gdkpixbuf);
            return false;
        }

        pixbuf = Glib::wrap(gdkpixbuf, true)->scale_simple(128, 128, Gdk::INTERP_BILINEAR);

        return pixbuf ? true : false;
    }

    bool get_theme_icon(WnckWindow* window, Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
    {
        gint32 xid = wnck_window_get_xid(window);

        // This matcher is owned by bamf and shared between other callers.
        BamfMatcher* matcher = bamf_matcher_get_default();
        if (!matcher) {
            g_warning("BamfMatcher: the object has not been created.");
            return false;
        }

        BamfApplication* bamfapp = bamf_matcher_get_application_for_xid(matcher, xid);
        if (!bamfapp) {
            g_warning("BamfApplication: the object has not been created.");
            return false;
        }

        const char* desktop_file = bamf_application_get_desktop_file(bamfapp);
        if (!desktop_file) {
            g_warning("desktop_file: the object has not been created.");

            return false;
        }

        Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
            Gio::DesktopAppInfo::create_from_filename(desktop_file);

        if (!appinfo) {
            g_warning("Gio::DesktopAppInfo: the object has not been created.");
            return false;
        }

        Glib::ustring title = appinfo->get_string("Name");
        g_print("%s\n", title.c_str());

        char* icon_name = g_desktop_app_info_get_string(appinfo->gobj(), "Icon");
        if (!icon_name) {
            g_warning(
                "g_desktop_app_info_get_string, icon_name : the object has not been created.");

            return false;
        }

        // get the default theme
        Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();
        if (!theme) {
            g_warning("Gtk::IconTheme: the object has not been created.");
            return false;
        }
        try {
            // Always get the icon scaled to the
            // requested size.
            pixbuf = theme->load_icon(icon_name, 128, Gtk::IconLookupFlags::ICON_LOOKUP_FORCE_SIZE);
        } catch (...) {
            g_warning("pixbuf: Exception the object has not been created. (%s)", icon_name);
            return false;
        }

        //       delete desktop_file;
        delete icon_name;

        return pixbuf ? true : false;
    }
#endif

    void AppProvider::on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        gint32 xid = wnck_window_get_xid(window);
        get_dockcontainer()->remove(xid);
    }

    void AppProvider::on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        gint32 xid = wnck_window_get_xid(window);

        auto container = get_dockcontainer();
        if (get_dockcontainer()->exist(xid)) return;

        WnckWindowType wt = wnck_window_get_window_type(window);

        // clang-format off
            if (wt == WNCK_WINDOW_DESKTOP ||
                wt == WNCK_WINDOW_DOCK ||
                wt == WNCK_WINDOW_TOOLBAR ||
                wt == WNCK_WINDOW_MENU ||
                wt == WNCK_WINDOW_SPLASHSCREEN){


                return ;
            }

        // clang-format on

        container->insert(window);

        // i/////////////////// TEST
        Glib::RefPtr<Gdk::Pixbuf> pixbuf;
        const char* group_name = wnck_window_get_class_group_name(window);

        if (get_window_icon(window, pixbuf)) {
            try {
                char filepath[512];
                sprintf(filepath,
                        "/home/yoo/TEMP/window/"
                        "%d-%s(%s)-%s",
                        xid, wnck_window_get_class_instance_name(window), group_name,
                        wnck_window_get_icon_name(window));
                pixbuf->save(filepath, "png");
            } catch (...) {
                // swallow
            }
        }
        return;
    }

    AppProvider::AppProvider()
    {
        WnckScreen* wnckscreen = wnck_screen_get_default();

        g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                         G_CALLBACK(&AppProvider::on_window_opened), nullptr);

        g_signal_connect(G_OBJECT(wnckscreen), "window-closed",
                         G_CALLBACK(&AppProvider::on_window_closed), nullptr);
    }
}  // namespace docklight
