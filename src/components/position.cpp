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
        return Gdk::Rectangle(0, 0, m_window->get_width(), m_window->get_height());
    }

    void PositionManager::force_position()
    {
        //
        // m_struts.reset_struts();
        m_struts.set_struts(true);
        //    set_position(1000);
        set_position(m_last_required_size);
    }

    void PositionManager::reset_position()
    {
        //
        //  m_lastposx = 0;
        m_struts.reset_struts();
        set_position(m_last_required_size);
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
        //    auto monitor = get_monitor();

        int xpos = 0, ypos = 0, center = 0;

        // vor the move setzen

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

            if (Config()->get_dock_location() == dock_location_t::bottom) {
                // if (!m_struts.is_set()) {
                // i
                //}
                // ypos = workarea.get_height() - !m_struts.is_set() ? 0 : area;
                // if (workarea.get_y() != 0) {
                // ypos = workarea.get_height() + workarea.get_y() - !m_struts.is_set() ? 0 : area;
                //}

                // auto offset = area;

                // if (m_struts.is_set()) {
                // offset = 0;
                //}
                // ypos = workarea.get_height() - offset;
                // if (workarea.get_y() != 0) {
                // ypos = workarea.get_height() + workarea.get_y() - offset;
                //}

                if (!m_struts.is_set()) {
                    // bottom
                    ypos = workarea.get_height() - area;
                    if (workarea.get_y() != 0) {
                        ypos = workarea.get_height() + workarea.get_y() - area;
                    }
                    g_print("-------------:NORMAL\n");
                } else {
                    // static int lastposx = 0;
                    ypos = workarea.get_height() + workarea.get_y() - area;

                    if (std::abs(m_lastposx - workarea.get_height()) == area) {
                        ypos = workarea.get_height() + workarea.get_y();
                        //  exit(1);
                    }

                    m_lastposx = workarea.get_height();

                    g_print("PPPPPPPPPPPPPPPPPPPPPPPPPPPP %d (%d) area %d lastposx %d\n", ypos,
                            workarea.get_height(), area, m_lastposx);
                    //  -area;
                    // - workarea.get_y();  // + workarea.get_y() - area;
                    if (ypos + area == workarea.get_height()) {
                        //

                        g_print("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n");
                        exit(1);
                    }

                    if (ypos < workarea.get_height()) {
                        //

                        ypos = workarea.get_height() + workarea.get_y();
                        g_print("AAAAAAAAAAAAAAAAAAAAAAi\n");
                    }
                    /*if (ypos < workarea.get_height() - area) {
                        //
                        ypos = workarea.get_height() + workarea.get_y();
                        g_print(
                            " 1  "
                            "-iSSSSSSSSSSSSSSSSSSS------------:STRUTS%d\n ",
                            ypos);
                        //} else [>if (ypos != workarea.get_height())<] {
                        // g_print(
                        //" 2  "
                        //"-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaas------------:STRUTS%"
                        //"d\n ",
                        // ypos);
                        // ypos = workarea.get_height() + workarea.get_y() - area;
                        g_print("AAAAAAAAAAAA %d == %d\n", ypos + area, workarea.get_height());
                        if (ypos + area == workarea.get_height()) {
                            ypos = workarea.get_height() + workarea.get_y();
                        }
                    } else {
                        g_print("XXXXXXXXXX %d == %d\n", ypos + area, workarea.get_height());
                        ypos = workarea.get_height() + workarea.get_y();
                    }*/
                }

                // ypos = workarea.get_height();  // - area + workarea.get_y();
                // if (workarea.get_y() != 0) {
                // if (ypos < workarea.get_height()) {
                // ypos = workarea.get_height() + workarea.get_y() - area;
                // g_print(
                //"-ssssssssssssssssssssssssssssssssssssssss------------:STRUTS%d\n ",
                // ypos);
                //} else {
                /////
                // ypos = workarea.get_height() + workarea.get_y() - area;
                // g_print("-aaaaaaaaaaaaaaaaaaaaaaas------------:STRUTS%d\n ", ypos);
                //}

                //}
                // g_print("-------------:STRUTS%d\n ", ypos);
            } else {
                // top
                if (workarea.get_y() > 0) {
                    ypos = workarea.get_y() - area;
                } else {
                    ypos = 0;
                }
            }

            m_window->resize(width, area);
            m_window->move(xpos, ypos);
            if (Config()->is_autohide_none()) {
                //       m_struts.set_struts();
            }

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

            if (Config()->get_dock_location() == dock_location_t::right) {
                // Right
                xpos = workarea.get_width();
                if (workarea.get_x() != 0) {
                    xpos = workarea.get_width() + workarea.get_x();
                }
            } else {
                // Left
                if (workarea.get_x() > 0) {
                    xpos = workarea.get_x() - area;
                } else {
                    xpos = 0;
                }
            }

            m_window->resize(area, height);
            m_window->move(xpos, ypos);
        }
    }
    //}  // namespace position

}  // namespace docklight

