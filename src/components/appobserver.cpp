#include "appobserver.h"

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

#endif

    bool AppObserver::valid_type(WnckWindow* window)
    {
        WnckWindowType wt = wnck_window_get_window_type(window);
        // clang-format off
            if (wt == WNCK_WINDOW_DESKTOP ||
                wt == WNCK_WINDOW_DOCK ||
                wt == WNCK_WINDOW_TOOLBAR ||
                wt == WNCK_WINDOW_MENU ||
                wt == WNCK_WINDOW_SPLASHSCREEN){

                return false;
            }
        // clang-format on

        return true;
    }

    void AppObserver::on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }
        if (!valid_type(window)) return;

        gint32 xid = wnck_window_get_xid(window);

        Provider()->remove(xid);
    }

    void AppObserver::on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        if (!valid_type(window)) return;

        gint32 xid = wnck_window_get_xid(window);

        auto provider = Provider();
        if (provider->exist(xid)) {
            return;
        }

        provider->insert(window);
    }

    AppObserver::AppObserver()
    {
        WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_PAGER);
        // WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_APPLICATION);
        WnckScreen* wnckscreen = wnck_handle_get_default_screen(handle);

        g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                         G_CALLBACK(&AppObserver::on_window_opened), nullptr);

        g_signal_connect(G_OBJECT(wnckscreen), "window-closed",
                         G_CALLBACK(&AppObserver::on_window_closed), nullptr);

        g_message("Create AppObserver.");
    }

    Glib::RefPtr<AppObserver> m_observer;
    Glib::RefPtr<AppObserver> create_observer()
    {
        if (!m_observer) {
            m_observer = Glib::RefPtr<AppObserver>(new AppObserver());
        }

        return m_observer;
    }

    Glib::RefPtr<AppObserver> Observer()
    {
        g_assert(m_observer);
        return m_observer;
    }

}  // namespace docklight
