#include "device.h"

DL_NS_BEGIN

namespace device
{
    namespace display
    {
        const Glib::RefPtr<Gdk::Display> get_default()
        {
            return Gdk::Display::get_default();
        }

        const Glib::RefPtr<Gdk::Screen> get_default_screen()
        {
            return Gdk::Display::get_default()->get_default_screen();
        }

        const Glib::RefPtr<Gdk::Screen> get_screen(int screen_num)
        {
            return Gdk::Display::get_default()->get_screen(screen_num);
        }

        int get_screen_count()
        {
            return display::get_default()->get_n_screens();
        }

    }  // namespace display

    namespace monitor
    {
        const Glib::RefPtr<Gdk::Monitor> get_primary()
        {
            return display::get_default()->get_primary_monitor();
        }

        const Glib::RefPtr<Gdk::Monitor> get_monitor(int monitor_num)
        {
            return display::get_default()->get_monitor(monitor_num);
        }

        int get_monitor_count()
        {
            return display::get_default()->get_n_monitors();
        }

    }  // namespace monitor

}  // namespace device
DL_NS_END
