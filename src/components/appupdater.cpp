#include "appupdater.h"

//#include <dirent.h>
//#include <giomm/appinfo.h>
//#include <giomm/desktopappinfo.h>
//#include <giomm/liststore.h>

//#include <glib-object.h>
//#include <iostream>
//#include <sstream>
//#include <thread>

//#include "gio/gdesktopappinfo.h"
/*Thanks, but I couldn't create a working solution so I came up with this one: Code :

    Glib::RefPtr<Gdk::Pixbuf> Info::getPixbuf(File* f)
{
    // File is a custom class
    static Glib::RefPtr<Gtk::IconTheme> iconTheme = Gtk::IconTheme::get_default();
    Glib::ustring sPath = Glib::build_filename(f->getDirPath(), f->getName());
    Glib::RefPtr<Gio::File> gioFile = Gio::File::create_for_path(sPath);
    Glib::RefPtr<Gio::FileInfo> info = gioFile->query_info();
    Glib::RefPtr<Gio::Icon> icon = info->get_icon();
    // getIconSize() a custom function returning the desired size
    Gtk::IconInfo iconInfo =
        iconTheme->lookup_icon(icon, getIconSize(), Gtk::ICON_LOOKUP_USE_BUILTIN);
    return iconInfo.load_icon();
}*/

namespace docklight
{
    void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        auto xid = wnck_window_get_xid(window);
        auto* dc = DockItemContainer::getInstance();
        dc->remove_entry(xid);
    }

    void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }

        DockItemContainer* dc = get_dockcontainer();
        gint32 xid = wnck_window_get_xid(window);
        if (dc->is_exist(xid)) return;

        // Gets the icon to be used for window. If no icon was found, a fallback icon is used
        // Returns :
        // the icon for window. The caller should reference the returned GdkPixbuf if it needs to
        // keep the icon around.
        GdkPixbuf* window_icon = wnck_window_get_icon(window);

        dc->add_entry(xid, wnck_window_get_name(window),
                      wnck_window_get_class_instance_name(window),
                      wnck_window_get_class_group_name(window), window_icon);
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
