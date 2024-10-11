//  Copyright (c) 2018-2024 Juan R. González
//
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  public Glib::Object GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  identification number, along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include "components/position.h"
// clang-format on
//#include <unistd.h>

//#include <cstdlib>
//#include <iostream>
namespace docklight
{

    // namespace position
    //{
    Glib::RefPtr<PositionManager> m_position_manager;

    Glib::RefPtr<PositionManager> create_position(Gtk::Window* window)
    {
        if (!m_position_manager) {
            m_position_manager = Glib::RefPtr<PositionManager>(new PositionManager(window));
        }
        return m_position_manager;
    }

    Glib::RefPtr<PositionManager> Position()
    {
        g_assert(m_position_manager);
        return m_position_manager;
    }

    PositionManager::PositionManager(Gtk::Window* window)
    {
        m_window = window;
        m_struts.init(window);

        g_message("Create PositionManager.");
    }

    const Gtk::Window* PositionManager::get_window() const
    {
        //
        return m_window;
    }

    Gdk::Rectangle PositionManager::get_workarea() const
    {
        return device::monitor::get_workarea();
    }

    Gdk::Rectangle PositionManager::get_monitor() const
    {
        return device::monitor::get_geometry();
    }

    Gdk::Rectangle PositionManager::get_window_geometry() const
    {
        // GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
        // auto gdk_window = gtk_widget_get_window(toplevel);

        // gint* x = nullptr;
        // gint* y = nullptr;
        // gint* width = nullptr;
        // gint* height = nullptr;
        //// https://docs.gtk.org/gdk3/method.Window.get_geometry.html
        //
        int yy = 0;
        int xx = 0;

        if (m_x) xx = m_x;
        if (m_y) yy = m_y;

        //        gdk_window_get_geometry(gdk_window, x, y, width, height);

        return Gdk::Rectangle(xx, yy, m_window->get_width(), m_window->get_height());
        // return Gdk::Rectangle(*x, *y, *width, *height);
    }

    /*
     * screen_width is the width to be calculated.
     * Like the width of a screen. The unit item_size multiplied by
     * items_count and divided by the screen_width gives us the resulting
     * factor that we can use as scale factor.
     *
     */
    inline double PositionManager::get_scalling_factor(const guint screen_width,
                                                       const guint item_size,
                                                       const guint items_count)
    {
        return screen_width / (double)(items_count * item_size);
    }

    double PositionManager::get_window_scalling_factor(const guint items_count, const guint area)
    {
        if (!m_window || !Config()) return 0;

        const auto workarea = device::monitor::get_workarea();

        int screen_width = workarea.get_width();
        int width = Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL
                        ? m_window->get_width()
                        : m_window->get_height();

        if (width > 1) {
            screen_width = width;
        };

        /*static int area = 0;

        if (!area) area = Config()->get_dock_area() + Config()->get_separator_size();
        // carea = area;*/

        screen_width -= area * 2;

        auto size = items_count;  // m_provider->data().size() + 1;
        auto icon_size = area;

        return screen_width / (double)(size * icon_size);
        // icon_size = std::floor(icon_size * scaling_factor);

        // g_print("XXXXXXXXXXX %d size: %ld\n", icon_size, m_provider->data().size());
        // if (icon_size >= area) {
        // icon_size = Config()->DEF_ICON_MAXSIZE;
        //}

        // return icon_size;
    }

    void PositionManager::monitor_changed()
    {
        //       m_struts.reset_struts();
        //   set_position(m_last_required_size);
        //    return;
        auto location_name = Config()->get_dock_location_name().c_str();
        // TODO program name after instllation can change
        // read the iall passing parameters
        g_print("Restart %s\n", location_name);
        execl("src/docklight", "docklight", "-l", location_name, NULL);

        // if (m_struts.active()) {
        // m_struts.set_struts();
        //}

        // set_position(m_last_required_size);
    }

    void PositionManager::force_position()
    {
        m_struts.set_struts();
        // set_position(m_last_required_size);
    }

    void PositionManager::reset_position()
    {
        m_struts.reset_struts();
    }
    void PositionManager::set_position(guint required_size)
    {
        // if (m_last_required_size == required_size) return;
        // m_last_required_size = required_size;

        g_message("Position request: %d", required_size);
        int area = Config()->get_dock_area();
        //   set_struts();
        dock_alignment_t alignment = Config()->get_dock_alignment();
        auto workarea = get_workarea();
        // auto monitor = get_monitor();

        guint xpos = 0, ypos = 0, center = 0;
        if (Config()->is_autohide_none()) {
            m_struts.set_struts();
        }

        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
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
                    break;
            }

            if (width <= 0) return;

            if (Config()->get_dock_location() == dock_location_t::bottom) {
                // bottom
                if (!m_struts.active()) {
                    ypos = workarea.get_height() - area;
                    if (workarea.get_y() != 0) {
                        ypos = workarea.get_height() + workarea.get_y() - area;
                    }
                } else {
                    ypos = m_struts.get_bottom_pos() - area;
                }

            } else {
                // top
                if (!m_struts.active()) {
                    ypos = workarea.get_y();
                } else {
                    ypos = m_struts.get_top_pos();  // + area;
                    // ypos = workarea.get_y() + area;
                }
            }

            m_window->resize(width, area);
            m_window->move(xpos, ypos);

            m_y = ypos;
            m_x = xpos;
            m_width = width;
            m_height = area;

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

            if (height > workarea.get_height()) return;

            if (Config()->get_dock_location() == dock_location_t::right) {
                // right
                if (!m_struts.active()) {
                    xpos = workarea.get_width() + workarea.get_x() - area;
                    /*xpos = workarea.get_width() - area;
                    if (workarea.get_x() != 0) {
                        xpos = workarea.get_width() + workarea.get_x() - area;
                    }*/
                } else {
                    xpos = m_struts.get_right_pos() - area;
                    // xpos = iworkarea.get_width();  // + workarea.get_x() - area;
                }
            } else {
                // left
                if (!m_struts.active()) {
                    xpos = workarea.get_x();

                    // if (workarea.get_x() > 0) {
                    // xpos = workarea.get_x();
                    //} else {
                    // xpos = 0;
                    //}
                } else {
                    xpos = m_struts.get_left_pos();

                    // xpos = workarea.get_x() - area;
                    //  if (xpos == workarea.get_x()) {
                    //  xpos = workarea.get_x();
                    // }

                    // if (xpos < workarea.get_x()) {
                    // xpos = workarea.get_x() - area;
                    // if (xpos < monitor.get_x()) {
                    // xpos = workarea.get_x();
                    //}
                    //}
                }
            }

            m_window->resize(area, height);
            m_window->move(xpos, ypos);
        }
    }
    //}  // namespace position

}  // namespace docklight

