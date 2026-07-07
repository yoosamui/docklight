#pragma once

#include <gdkmm/pixbuf.h>
#include <libwnck/libwnck.h>

#include "components/config.h"
#include "components/position.h"

#include <gdk/gdkx.h>

namespace docklight
{
    namespace pixbuf
    {

        bool xcomposite_supported();

        void invert_pixels(Glib::RefPtr<Gdk::Pixbuf> image);

        const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string &filename, int width,
                                                      int height);

        const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(WnckWindow *window,
                                                        const std::string &icon_name, int size);

        const Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf *icon);

        bool get_window_image(gulong xid, Glib::RefPtr<Gdk::Pixbuf> &image,
                              guint size = Config()->get_preview_image_size());

        GdkPixbuf *get_gdk_pixbuf_scaled(gulong xid, const GdkPixbuf *pixbuf, const guint destWidth,
                                         const guint destHeight);

        Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_scaled(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf,
                                                    const guint destWidth, const guint destHeight,
                                                    guint &scaledWidth, guint &scaledHeight);

        int compare_pixels(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf_a,
                           const Glib::RefPtr<Gdk::Pixbuf> &pixbuf_b, bool validate);

        int compare_pixels(const GdkPixbuf *pixbuf_a, const GdkPixbuf *pixbuf_b, bool validate);

        const Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf *icon);

    } // namespace pixbuf
} // namespace docklight
