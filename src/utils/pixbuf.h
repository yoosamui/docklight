#pragma once

// clang-format off
//o#include <glibmm/fileutils.h>
//#include <gdkmm.h>
#include <gtkmm/iconinfo.h>
#include <gtkmm/icontheme.h>
//#include "/usr/include/gtkmm-3.0/icontheme.h"
#include <X11/Xlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
// clang-format on
//
namespace docklight
{
    namespace pixbuf
    {
        const Glib::RefPtr<Gdk::Pixbuf> get_theme_icon(const gchar* icon_name,
                                                       GdkPixbuf* window_icon, guint size = 128);

        const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(GdkPixbuf* icon, guint size = 128);
        // const Glib::RefPtr<Gdk::Pixbuf> convert_gioicon(const Glib::RefPtr<Gio::Icon>& icon,
        // guint size = 128);

    }  // namespace pixbuf
}  // namespace docklight
