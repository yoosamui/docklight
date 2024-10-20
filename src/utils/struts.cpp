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
#include "utils/struts.h"
// clang-format on

namespace docklight::position
{

    Struts::Struts()
    {
        g_message("create Struts.");
    }

    Struts::~Struts()
    {
        reset_struts();
    }

    void Struts::init(Gtk::Window* window)
    {
        m_window = window;
    }

    void Struts::reset_struts()
    {
        long insets[12] = {0};

        set_insets(*insets);

        m_strut_top_pos = -1;
        m_strut_right_pos = -1;
        m_strut_left_pos = -1;
        m_strut_bottom_pos = -1;
        m_active = false;

        g_print("Reset struts\n");
    }

    void Struts::set_insets(long& insets)
    {
        GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
        auto gdk_window = gtk_widget_get_window(toplevel);
        if (!gdk_window) {
            g_warning("set_strut: gdk_window is null.");
            return;
        }
        gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                            gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                            (unsigned char*)&insets, 12);

        gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT", FALSE),
                            gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                            (unsigned char*)&insets, 4);
    }

    bool Struts::active() const
    {
        return m_active;
    }

    guint Struts::get_bottom_pos() const
    {
        return m_last_bottom_pos;
    }

    guint Struts::get_right_pos() const
    {
        return m_last_right_pos;
    }

    guint Struts::get_left_pos() const
    {
        return m_last_left_pos;
    }

    guint Struts::get_top_pos() const
    {
        return m_last_top_pos;
    }

    void Struts::set_struts()
    {
        long insets[12] = {0};

        auto const screen = device::display::get_default_screen();
        auto const location = Config()->get_dock_location();
        auto area = Config()->get_dock_area();
        Gdk::Rectangle workarea = device::monitor::get_workarea();
        Gdk::Rectangle monitor = device::monitor::get_geometry();

        switch (location) {
            // clang-format off
                case dock_location_t::top:
                        if (m_strut_top_pos == -1) {
                            m_strut_top_pos = workarea.get_y();
                            m_last_top_pos = workarea.get_y();
                        }

                        insets[struts_position_t::top] = m_strut_top_pos + area;
                        insets[struts_position_t::top_start] = monitor.get_x();
                        insets[struts_position_t::top_end] = monitor.get_x() + monitor.get_width();

                        break;
                case dock_location_t::bottom:
                        if (m_strut_bottom_pos == -1) {
                            m_strut_bottom_pos = screen->get_height()  -  workarea.get_y() - workarea.get_height();
                            m_last_bottom_pos =  workarea.get_height() + workarea.get_y();
                        }

                        insets[struts_position_t::bottom] = m_strut_bottom_pos +  area  ;
                        insets[struts_position_t::bottom_start] = monitor.get_x();
                        insets[struts_position_t::bottom_end] = monitor.get_x()+monitor.get_height();

                        break;
                case dock_location_t::left:

                        if (m_strut_left_pos == -1) {
                            m_strut_left_pos = workarea.get_x();
                            m_last_left_pos = workarea.get_x();
                        }

                        insets[struts_position_t::left] = m_strut_left_pos + area ;
                        insets[struts_position_t::left_start] = monitor.get_y();
                        insets[struts_position_t::left_end] = monitor.get_y() + monitor.get_height();

                        break;
                case dock_location_t::right:

                        if (m_strut_right_pos == -1) {
                            m_strut_right_pos = screen->get_width()  - workarea.get_width() - workarea.get_x();
                            m_last_right_pos =  workarea.get_width() + workarea.get_x();
                        }

                        insets[struts_position_t::right] = m_strut_right_pos +  area;
                        insets[struts_position_t::right_start] = monitor.get_y();
                        insets[struts_position_t::right_end] = monitor.get_height();

                        break;
                default:
                        break;
                // clang-format on
        }

        set_insets(*insets);

        m_active = true;
        //   g_message("set_strut.");
    }

}  // namespace docklight::position
