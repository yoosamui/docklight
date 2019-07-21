#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <gdkmm/pixbuf.h>
#include <libwnck/libwnck.h>
#include "common.h"

DL_NS_BEGIN

namespace pixbuf_util
{
    const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string& filename,
                                                  int width, int height);

    const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(WnckWindow* window,
                                                    const string& icon_name,
                                                    int size);

    const Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon);

}  // namespace pixbuf_util

DL_NS_END

