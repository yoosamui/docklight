#pragma once

// clang-format off
#include <gtkmm/window.h>

#include "components/device.h"
#include "components/config.h"
#include "utils/struts.h"
// clang-format on
//
// TODO: for testing
//#include <libwnck/libwnck.h>

//#include "utils/wnck.h"

namespace docklight
{

    namespace position
    {

        class PositionManager : public Struts
        {
          public:
            PositionManager(Gtk::Window* window) : Struts(window)
            {
                //
                m_window = window;
                g_print("......................Welcome Juan in PositionManager\n");
            }

            const Gtk::Window* get_window() const
            {
                //
                return m_window;
            }

            Gdk::Rectangle get_workarea() const
            {
                //

                return device::monitor::get_workarea();
            }

            void set_position(guint required_size)
            {
                if (m_last_required_size == required_size) return;
                m_last_required_size = required_size;
                //
                int m_last_width = 0;
                int m_last_height = 0;
                bool m_strut_set = false;

                int area = config::get_dock_area();

                dock_alignment_t alignment = config::get_dock_alignment();
                auto workarea = get_workarea();

                int xpos = 0, ypos = 0, center = 0;
                //    m_strut_set = false;

                if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                    int width = required_size;

                    if (width > workarea.get_width()) {
                        width = workarea.get_width();
                    }

                    switch (alignment) {
                        case dock_alignment_t::start:
                            xpos = workarea.get_x();
                            break;

                        case dock_alignment_t::end:
                            xpos = workarea.get_x() + workarea.get_width() - width;
                            break;

                        case dock_alignment_t::center:
                            center = workarea.get_width() / 2 - width / 2;
                            xpos = workarea.get_x() + center;
                            break;

                        default:  // fill
                            xpos = workarea.get_x();
                            width = workarea.get_width();
                    }

                    if (config::get_dock_location() == dock_location_t::bottom) {
                        ypos = workarea.get_y() + workarea.get_height() - area;
                    } else {
                        ypos = workarea.get_y();
                    }

                    if (config::is_autohide_none()) {
                        set_struts();
                    }

                    m_window->resize(width, area);
                    m_window->move(xpos, ypos);

                } else  // orientation vertical
                {
                    int height = required_size;
                    //
                    if (height > workarea.get_height()) {
                        height = workarea.get_height();
                    }

                    switch (alignment) {
                        case dock_alignment_t::start:
                            ypos = workarea.get_y();
                            break;

                        case dock_alignment_t::end:
                            ypos = workarea.get_y() + workarea.get_height() - height;
                            break;

                        case dock_alignment_t::center:
                            center = workarea.get_height() / 2 - height / 2;
                            ypos = workarea.get_y() + center;
                            break;

                        default:  // fill
                            ypos = workarea.get_y();
                            height = workarea.get_height();
                            break;
                    }

                    if (config::get_dock_location() == dock_location_t::right) {
                        // xpos = workarea.get_x() + workarea.get_width() - area;
                        xpos = workarea.get_width() - area;
                    } else {
                        xpos = workarea.get_x();
                    }

                    if (config::is_autohide_none()) {
                        set_struts();
                    }

                    m_window->resize(area, height);
                    m_window->move(xpos, ypos);
                }
            }

          private:
            Gtk::Window* m_window;
            int m_last_required_size = 0;
        };

        Glib::RefPtr<PositionManager> create(Gtk::Window* window);

        //////
        void get_docks();
        void init(Gtk::Window& window);
        void set_window_position(guint required_size);

        Gdk::Rectangle get_monitor_geometry();
        Gdk::Rectangle get_background_region();
        Gdk::Rectangle get_workarea();

        const Gtk::Window* get_window();

        void update_client_geometry(int& x, int& y, int& width, int& height);
        // namespace strutsX
        //{
        // void set_strut(bool reset);
        //}
        namespace struts
        {
            void set_strut(bool create = true);

        }  // namespace struts
    }      // namespace position

}  // namespace docklight
