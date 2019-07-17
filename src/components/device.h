#pragma once

#include <gdkmm/display.h>
#include <gdkmm/monitor.h>
#include <gdkmm/screen.h>
#include "common.h"

DL_NS_BEGIN

namespace device
{
    namespace display
    {
        const Glib::RefPtr<Gdk::Display> get_default();
        const Glib::RefPtr<Gdk::Screen> get_default_screen();
        const Glib::RefPtr<Gdk::Screen> get_screen(int screen_num);
        int get_screen_count();

    }  // namespace display

    namespace monitor
    {
        const Glib::RefPtr<Gdk::Monitor> get_primary();
        const Glib::RefPtr<Gdk::Monitor> get_monitor(int monitor_num);
        int get_monitor_count();

    }  // namespace monitor
}  // namespace device
DL_NS_END
