#pragma once

#include <gdkmm/pixbuf.h>

#include <gdk/gdkx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>

namespace docklight::capture
{
    struct WindowGeometry
    {
        Window root;

        int x;
        int y;

        unsigned int width;
        unsigned int height;

        unsigned int border;
        unsigned int depth;
    };

    GdkPixbuf *capture_window(gulong xid);
    bool query_geometry(gulong xid, WindowGeometry &geo);
    GdkPixbuf *convert_bgra_to_pixbuf(XImage *img, const WindowGeometry &geo);
    bool is_standard_bgra(const XImage *img);
    XImage *capture_ximage(gulong xid, const WindowGeometry &geo);
    Display *get_display();
}
