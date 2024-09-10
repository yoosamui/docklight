#pragma once

// clang-format off
//#include <glibmm/fileutils.h>
#include <gtkmm/iconinfo.h>
#include <gtkmm/icontheme.h>
#include <X11/Xlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
// clang-format on
//
namespace docklight
{
    namespace pixbuf
    {
        const Glib::RefPtr<Gdk::Pixbuf> convert_window_icon(GdkPixbuf* icon, guint size = 128);
        const Glib::RefPtr<Gdk::Pixbuf> convert_gioicon(const Glib::RefPtr<Gio::Icon>& icon,
                                                        guint size = 128);

    }  // namespace pixbuf
}  // namespace docklight
