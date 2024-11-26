#pragma once
#include <gdkmm/display.h>
#include <gdkmm/monitor.h>
#include <gdkmm/rectangle.h>
#include <gdkmm/screen.h>

namespace docklight
{
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
            const Glib::RefPtr<Gdk::Monitor> get_monitor();
            const Glib::RefPtr<Gdk::Monitor> get_current();

            void set_current_monitor(Glib::ustring name);
            void set_current_monitor(int monitor_num);

            int get_monitor_number();
            Gdk::Rectangle get_workarea();
            Gdk::Rectangle get_geometry();
            int get_monitor_count();
            void set_primary();

        }  // namespace monitor
    }      // namespace device
}  // namespace docklight
