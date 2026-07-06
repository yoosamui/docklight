#include "components/capture.h"

#include <gdk/gdkx.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrender.h>

#include <cstring>
#include <iostream>

namespace docklight::capture
{
    GdkPixbuf *capture_window(gulong xid)
    {

        GdkDisplay *gdk_display = gdk_display_get_default();

        if (!gdk_display)
            return nullptr;

        Display *display = GDK_DISPLAY_XDISPLAY(gdk_display);

        int eventBase, errorBase;

        if (!XCompositeQueryExtension(display, &eventBase, &errorBase))
            return nullptr;

        gdk_x11_display_error_trap_push(gdk_display);

        // We are NOT using pixmap anymore
        Window win = (Window)xid;

        gdk_x11_display_error_trap_pop_ignored(gdk_display);

        if (win == None)
            return nullptr;

        // =====================================================
        // SAFE PIXEL CAPTURE
        // =====================================================

        Window root;
        int x, y;
        unsigned int width, height, border, depth;

        if (!XGetGeometry(display, win, &root, &x, &y, &width, &height, &border, &depth))
            return nullptr;

        if (width == 0 || height == 0)
            return nullptr;

        XImage *img = XGetImage(
            display,
            win,
            0, 0,
            width,
            height,
            AllPlanes,
            ZPixmap);

        if (!img)
            return nullptr;

        GdkPixbuf *pixbuf =
            gdk_pixbuf_new(
                GDK_COLORSPACE_RGB,
                TRUE,
                8,
                width,
                height);

        if (!pixbuf)
        {
            XDestroyImage(img);
            return nullptr;
        }

        // =====================================================
        // FIXED COPY LOOP (COLOR CORRECTION HERE)
        // =====================================================

        guchar *dst = gdk_pixbuf_get_pixels(pixbuf);
        guchar *src = (guchar *)img->data;

        int dst_stride = gdk_pixbuf_get_rowstride(pixbuf);
        int src_stride = img->bytes_per_line;

        for (unsigned int y = 0; y < height; y++)
        {
            guchar *d = dst + y * dst_stride;
            guchar *s = src + y * src_stride;

            for (unsigned int x = 0; x < width; x++)
            {
                guchar b = s[x * 4 + 0];
                guchar g = s[x * 4 + 1];
                guchar r = s[x * 4 + 2];
                guchar a = s[x * 4 + 3];

                d[x * 4 + 0] = r;
                d[x * 4 + 1] = g;
                d[x * 4 + 2] = b;
                d[x * 4 + 3] = a;
            }
        }

        XDestroyImage(img);

        return pixbuf;
    }
}