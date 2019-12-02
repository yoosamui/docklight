#include "device.h"

DL_NS_BEGIN

namespace device
{
    namespace display
    {
        const Glib::RefPtr<Gdk::Display> get_default() { return Gdk::Display::get_default(); }

        const Glib::RefPtr<Gdk::Screen> get_default_screen()
        {
            return Gdk::Display::get_default()->get_default_screen();
        }

        const Glib::RefPtr<Gdk::Screen> get_screen(int screen_num)
        {
            return Gdk::Display::get_default()->get_screen(screen_num);
        }

        int get_screen_count() { return display::get_default()->get_n_screens(); }

    }  // namespace display

    namespace monitor
    {
        int m_monitor_number = 0;
        // unique_ptr<Current> m_current = nullptr;

        const Glib::RefPtr<Gdk::Monitor> get_current()
        {
            //
            return get_monitor();
        }

        const Glib::RefPtr<Gdk::Monitor> get_primary()
        {
            return display::get_default()->get_primary_monitor();
        }

        const Glib::RefPtr<Gdk::Monitor> get_monitor(int monitor_num)
        {
            return display::get_default()->get_monitor(monitor_num);
        }

        // get the current monitor
        const Glib::RefPtr<Gdk::Monitor> get_monitor()
        {
            auto monitor = display::get_default()->get_monitor(m_monitor_number);
            if (monitor) {
                return monitor;
            }

            for (int i = 0; i < get_monitor_count(); i++) {
                monitor = get_monitor(i);
                if (monitor) {
                    set_current_monitor(i);
                    return monitor;
                }
            }

            monitor = display::get_default()->get_primary_monitor();
            if (monitor) {
                set_primary();
                return monitor;
            }

            g_error("no monitor available %d\n", get_monitor_count());
            return (Glib::RefPtr<Gdk::Monitor>)nullptr;
        }

        int get_monitor_number()
        {  //
            return m_monitor_number;
        }

        void set_current_monitor(int monitor_num)
        {
            if (monitor_num > get_monitor_count()) {
                g_error("Invalid monitor number\n");
                return;
            }

            m_monitor_number = monitor_num;
        }

        Gdk::Rectangle get_workarea()
        {
            Gdk::Rectangle workarea;
            get_monitor()->get_workarea(workarea);

            return workarea;
        }

        Gdk::Rectangle get_geometry()
        {
            Gdk::Rectangle geometry;
            get_monitor()->get_geometry(geometry);

            return geometry;
        }

        int get_monitor_count()
        {  //
            return display::get_default()->get_n_monitors();
        }

        void set_primary()
        {
            for (int i = 0; i < get_monitor_count(); i++) {
                auto m = get_monitor(i);
                if (m->is_primary()) {
                    m_monitor_number = i;
                    break;
                }
            }
        }
        // void create_strut(dock_location_t location) {}

        // const unique_ptr<Current>& get_current()
        //{
        // if (!m_current) {
        // m_current = unique_ptr<Current>(new Current);
        //}

        // return m_current;
        //}

        // Current::Current()
        //{
        // for (int i = 0; i < get_monitor_count(); i++) {
        // auto m = get_monitor(i);
        // if (m->is_primary()) {
        // m_monitor_number = i;
        // break;
        //}
        //}
        //}

        //// const Glib::RefPtr<Gdk::Monitor> Current::get_monitor_obj()
        ////{
        //// return get_monitor(m_monitor_number);
        ////}

        // void Current::set_primary()
        //{
        // for (int i = 0; i < get_monitor_count(); i++) {
        // auto m = get_monitor(i);
        // if (m->is_primary()) {
        // m_monitor_number = i;
        // break;
        //}
        //}
        //}
        // void Current::set_current_monitor(int monitor_num)
        //{
        // if (monitor_num > get_monitor_count()) {
        // g_error("Invalid monitor number\n");
        // return;
        //}

        // m_monitor_number = monitor_num;
        //}

        // Gdk::Rectangle Current::get_workarea()
        //{
        // Gdk::Rectangle workarea;
        // auto const monitor = get_monitor(m_monitor_number);
        // monitor->get_workarea(workarea);
        // return workarea;
        //}

        // Gdk::Rectangle Current::get_geometry()
        //{
        // Gdk::Rectangle geometry;
        // auto const monitor = get_monitor(m_monitor_number);
        // monitor->get_geometry(geometry);
        // return geometry;
        //}

        // int Current::get_monitor_number() { return m_monitor_number; }

    }  // namespace monitor

}  // namespace device
DL_NS_END
