#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <gdkmm/pixbuf.h>
#include <libwnck/libwnck.h>
#include "common.h"

DL_NS_BEGIN

namespace pixbuf_util
{
#define NULLPB (Glib::RefPtr<Gdk::Pixbuf>)nullptr

    void invert_pixels(Glib::RefPtr<Gdk::Pixbuf> image);

    const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string& filename, int width,
                                                  int height);

    const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(WnckWindow* window, const string& icon_name,
                                                    int size);

    const Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon);

    const Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_from_window(int xid);

    Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_scaled(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf,
                                                const guint destWidth, const guint destHeight,
                                                guint& scaledWidth, guint& scaledHeight);

    int compare_pixels(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_a,
                       const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_b);

}  // namespace pixbuf_util

DL_NS_END

