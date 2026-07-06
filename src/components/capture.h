#pragma once

#include <gdkmm/pixbuf.h>




namespace docklight::capture
{
    GdkPixbuf *capture_window(gulong xid);
}
