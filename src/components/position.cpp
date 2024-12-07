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
#include <unistd.h>
#include <stdio.h>
#include <linux/limits.h>
// clang-format on

namespace docklight
{
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

        m_exepath = get_execpath();
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

    int PositionManager::get_x() const
    {
        return m_x;
    }

    int PositionManager::get_y() const
    {
        return m_y;
    }

    Gdk::Rectangle PositionManager::get_window_geometry() const
    {
        return Gdk::Rectangle(m_x, m_y, m_window->get_width(), m_window->get_height());
    }

    std::string PositionManager::get_execpath()
    {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        return std::string(result, (count > 0) ? count : 0);
    }

    void PositionManager::window_intersects(bool intersects)
    {
        if (intersects && !m_lock) {
            m_lock = true;
            if (Config()->is_autohide_none()) {
                m_struts.reset_struts();
            }
        } else if (m_lock) {
            m_lock = false;
            if (Config()->is_autohide_none()) {
                m_struts.set_struts();
            }
        }
    }

    void PositionManager::show_now()
    {
        //      m_window->set_visible(true);
        //..  m_window->show();

        // g_print("S H O W\n");
    }

    void PositionManager::hide_now()
    {
        // m_window->set_visible(false);

        // m_window->hide();
    }

    void PositionManager::on_monitor_changed()
    {
        unsigned int microsecond = 1000000;

        auto monitor_name = Config()->get_monitor_name().c_str();
        auto location_name = Config()->get_dock_location_name().c_str();
        auto exec_file = "docklight";

        size_t found = m_exepath.find("/sources/docklight");
        if (found != std::string::npos) {
            exec_file = "src/docklight";
        }

        usleep(3 * microsecond);  // sleeps for 3 seconds
        execl(exec_file, "docklight", "-l", location_name, "-m", monitor_name, nullptr);
        g_warning("Restart failed!\n");
    }

    bool PositionManager::get_dockmenu_position(int index, int& x, int& y, int width, int height)
    {
        auto const area = Config()->get_dock_area();
        auto workarea = get_workarea();

        x = m_x;
        y = m_y;

        int maxsize = area * Provider()->data().size();

        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            if (Config()->get_dock_alignment() == dock_alignment_t::fill) {
                if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::center) {
                    int center = workarea.get_width() / 2 - maxsize / 2;
                    x = workarea.get_x() + center;

                } else if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::end) {
                    x = workarea.get_width() - maxsize;
                }
            }

            int center = (area / 2) - (width / 2);
            x += (area * index) + center;

            if (Config()->get_dock_location() == dock_location_t::top) {
                y = m_y + area;  // + height;
            } else {
                y = m_y - height;
            }

        } else {  // Vertical
            if (Config()->get_dock_alignment() == dock_alignment_t::fill) {
                if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::center) {
                    int center = workarea.get_height() / 2 - maxsize / 2;
                    y = workarea.get_y() + center;
                } else if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::end) {
                    y = workarea.get_height() + workarea.get_y() - maxsize;
                }
            }

            int center = (area / 2) - (height / 2);
            y += (area * index) + center;

            if (Config()->get_dock_location() == dock_location_t::right) {
                // x = workarea.get_x() + workarea.get_width() - area;
                // x = m_x;  // workarea.get_x() + workarea.get_width() - width;
                x -= width;
            } else {
                x += area;
            }
        }

        return true;
    }
    // int area = Config()->get_preview_area();
    // int size = area * dockitem->get_childmap().size();
    // Position()->get_preview_position(m_dockitem_index, x, y, size, area);

    bool PositionManager::get_preview_position(int index, int& x, int& y, int width, int height)
    {
        auto const area = Config()->get_dock_area();
        auto workarea = get_workarea();

        x = m_x;
        y = m_y;  //- height;

        int maxsize = area * Provider()->data().size();
        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            if (Config()->get_dock_alignment() == dock_alignment_t::fill) {
                if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::center) {
                    int center = workarea.get_width() / 2 - maxsize / 2;
                    x = workarea.get_x() + center;

                } else if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::end) {
                    x = workarea.get_width() - maxsize;
                }
            }

            int center = (area / 2) - (width / 2);
            x += (area * index) + center;
            y -= height;

            if (x < workarea.get_x()) {
                x = workarea.get_x();
            }

            if (x + width > workarea.get_x() + workarea.get_width()) {
                x = workarea.get_x() + workarea.get_width() - width;
            }

            if (Config()->get_dock_location() == dock_location_t::top) {
                y += area + height;
            }

        } else {  // Vertical
            if (Config()->get_dock_alignment() == dock_alignment_t::fill) {
                if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::center) {
                    int center = workarea.get_height() / 2 - maxsize / 2;
                    y = workarea.get_y() + center;
                } else if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::end) {
                    y = workarea.get_height() + workarea.get_y() - maxsize;
                }
            }

            int center = (area / 2) - (height / 2);
            y += (area * index) + center;

            if (y < workarea.get_y()) {
                y = workarea.get_y();
            }

            if (y + height > workarea.get_y() + workarea.get_height()) {
                y = workarea.get_y() + get_workarea().get_height() - height;
            }

            if (Config()->get_dock_location() == dock_location_t::right) {
                // x = workarea.get_x() + workarea.get_width() - area;
                x = workarea.get_x() + workarea.get_width() - width;
            } else {
                x += area;
            }
        }

        return true;
    }
    void PositionManager::force_position()
    {
        m_struts.set_struts();
    }

    void PositionManager::reset_position()
    {
        m_struts.reset_struts();
    }
    void PositionManager::set_position(guint required_size)
    {
        if (m_lock) return;
        m_last_required_size = required_size;

        int area = Config()->get_dock_area();

        dock_alignment_t alignment = Config()->get_dock_alignment();
        auto workarea = get_workarea();
        // auto monitor = get_monitor();

        guint xpos = 0, ypos = 0, center = 0;

        if (Config()->is_autohide_none()) {
            m_struts.set_struts();
        }

        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int width = required_size;
            if (width <= 0) return;

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
                    ypos = m_struts.get_top_pos();
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
            if (height <= 0) return;

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

            if (Config()->get_dock_location() == dock_location_t::right) {
                // right
                if (!m_struts.active()) {
                    xpos = workarea.get_width() + workarea.get_x() - area;
                } else {
                    xpos = m_struts.get_right_pos() - area;
                }
            } else {
                // left
                if (!m_struts.active()) {
                    xpos = workarea.get_x();
                } else {
                    xpos = m_struts.get_left_pos();
                }
            }

            m_window->resize(area, height);
            m_window->move(xpos, ypos);

            m_y = ypos;
            m_x = xpos;
            m_width = area;
            m_height = height;
        }
    }

}  // namespace docklight

