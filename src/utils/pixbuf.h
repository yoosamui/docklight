#pragma once

#include <gdkmm/pixbuf.h>
#include "common.h"

DL_NS_BEGIN

namespace pixbuf_util
{
    const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string& filename,
                                                  int width, int height);

}  // namespace pixbuf_util

DL_NS_END

