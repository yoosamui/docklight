#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <gdkmm/pixbuf.h>
#include <libwnck/libwnck.h>
#include "common.h"

DL_NS_BEGIN

#define NULLPB (Glib::RefPtr<Gdk::Pixbuf>)nullptr
namespace pixbuf_util
{
    void invert_pixels(Glib::RefPtr<Gdk::Pixbuf> image);

    const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string& filename, int width,
                                                  int height);

    const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(WnckWindow* window, const string& icon_name,
                                                    int size);

    const Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon);

    GdkPixbuf* get_pixbuf_from_window_raw(int xid);
    const Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_from_window(int xid);
    const Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_from_window(int xid, int width, int height);
    GdkPixbuf* get_gdk_pixbuf_from_window(int xid);

    GdkPixbuf* get_gdk_pixbuf_scaled(const GdkPixbuf* pixbuf, const guint destWidth,
                                     const guint destHeight);

    Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_scaled(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf,
                                                const guint destWidth, const guint destHeight,
                                                guint& scaledWidth, guint& scaledHeight);

    int compare_pixels(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_a,
                       const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_b, bool validate);

    int compare_pixels(const GdkPixbuf* pixbuf_a, const GdkPixbuf* pixbuf_b, bool validate);
}  // namespace pixbuf_util

DL_NS_END

