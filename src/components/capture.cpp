#include "components/capture.h"

#include <cstring>
#include <iostream>

namespace docklight::capture
{

    Display *get_display()
    {
        GdkDisplay *gdk_display = gdk_display_get_default();

        if (!gdk_display)
            return nullptr;

        return GDK_DISPLAY_XDISPLAY(gdk_display);
    }

    bool is_standard_bgra(const XImage *img)
    {

        return img->bits_per_pixel == 32 &&
               img->byte_order == LSBFirst &&
               img->red_mask == 0x00ff0000 &&
               img->green_mask == 0x0000ff00 &&
               img->blue_mask == 0x000000ff;
    }

    GdkPixbuf *convert_bgra_to_pixbuf(XImage *img, const WindowGeometry &geo)
    {
        GdkPixbuf *pixbuf =
            gdk_pixbuf_new(
                GDK_COLORSPACE_RGB,
                TRUE,
                8,
                geo.width,
                geo.height);

        if (!pixbuf)
        {
            return nullptr;
        }

        // Convert XImage -> GdkPixbuf

        guchar *dst = gdk_pixbuf_get_pixels(pixbuf);
        guchar *src = reinterpret_cast<guchar *>(img->data);

        int dst_stride = gdk_pixbuf_get_rowstride(pixbuf);
        int src_stride = img->bytes_per_line;

        for (unsigned int y = 0; y < geo.height; y++)
        {
            guchar *d = dst + y * dst_stride;
            guchar *s = src + y * src_stride;

            for (unsigned int x = 0; x < geo.width; x++)
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

        return pixbuf;
    }

    bool query_geometry(gulong xid, WindowGeometry &geo)
    {
        GdkDisplay *gdk_display = gdk_display_get_default();
        if (!gdk_display)
            return false;

        // Display *display = GDK_DISPLAY_XDISPLAY(gdk_display);
        Display *display = get_display();

        if (!display)
            return false;

        Window win = static_cast<Window>(xid);

        return XGetGeometry(display,
                            win,
                            &geo.root,
                            &geo.x,
                            &geo.y,
                            &geo.width,
                            &geo.height,
                            &geo.border,
                            &geo.depth);
    }

    XImage *capture_ximage(gulong xid, const WindowGeometry &geo)
    {
        GdkDisplay *gdk_display = gdk_display_get_default();

        if (!gdk_display)
            return nullptr;

        // Display *display = GDK_DISPLAY_XDISPLAY(gdk_display);
        Display *display = get_display();

        if (!display)
            return nullptr;

        Window win = static_cast<Window>(xid);

        return XGetImage(
            display,
            win,
            0,
            0,
            geo.width,
            geo.height,
            AllPlanes,
            ZPixmap);
    }

    GdkPixbuf *capture_window(gulong xid)
    {
        WindowGeometry geo;

        if (!query_geometry(xid, geo))
            return nullptr;

        if (geo.width == 0 || geo.height == 0)
            return nullptr;

        XImage *img = capture_ximage(xid, geo);

        if (!img)
            return nullptr;

        if (!is_standard_bgra(img))
        {
            XDestroyImage(img);
            return nullptr;
        }

        GdkPixbuf *pixbuf = convert_bgra_to_pixbuf(img, geo);

        XDestroyImage(img);

        return pixbuf;
    }

}