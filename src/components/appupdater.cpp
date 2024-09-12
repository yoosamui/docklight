#include "appupdater.h"

namespace docklight
{
    void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        gint32 xid = wnck_window_get_xid(window);
        get_dockcontainer()->remove(xid);
    }

    void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        DockItemContainer* container = get_dockcontainer();
        gint32 xid = wnck_window_get_xid(window);
        if (get_dockcontainer()->is_exist(xid)) return;

        GdkPixbuf* window_icon = wnck_window_get_icon(window);
        const char* window_icon_name = wnck_window_get_icon_name(window);

        // clang-format off
        container->insert(
            xid,
            wnck_window_get_name(window),
            wnck_window_get_class_instance_name(window),
            wnck_window_get_class_group_name(window),
            window_icon_name,
            window_icon);
        // clang-format on

        // TODO causes Gelib Errors
        // GdkPixbuf* window_icon, when it no longer needs the pixbuf, it should release the
        // reference it acquired by calling g_object_unref(). The resources associated with a
        // GdkPixbuf will be freed when its reference count drops to zero.
        // if (G_IS_OBJECT(window_icon)) g_object_unref(window_icon);
    }

    AppProvider::AppProvider()
    {
        WnckScreen* wnckscreen = wnck_screen_get_default();

        g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(&on_window_opened),
                         nullptr);

        g_signal_connect(G_OBJECT(wnckscreen), "window-closed", G_CALLBACK(&on_window_closed),
                         nullptr);
    }
}  // namespace docklight
