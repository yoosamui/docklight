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
        //
    }

    void Struts::init(Gtk::Window* window)
    {
        m_window = window;
    }

    void Struts::set_struts(bool force)
    {
        if (m_strut_set && !force) return;

        long insets[12] = {0};

        auto const screen = device::display::get_default_screen();
        auto const location = Config()->get_dock_location();
        auto area = Config()->get_dock_area();
        auto workarea = device::monitor::get_workarea();
        auto scale_factor = 1;

        switch (location) {
                // clang-format off
                case dock_location_t::top:
                        insets[struts_position_t::top] = workarea.get_y() + area * scale_factor;
                        insets[struts_position_t::top_start] = workarea.get_x() * scale_factor;
                        insets[struts_position_t::top_end] = (workarea.get_x() + workarea.get_width()) * scale_factor -1;
                    break;
                case dock_location_t::bottom:
                        insets[struts_position_t::bottom] =  (area + screen->get_height() - workarea.get_y() -   workarea.get_height()) *     scale_factor;
                        insets[struts_position_t::bottom_start] = workarea.get_y() * scale_factor;
                        insets[struts_position_t::bottom_end] = (workarea.get_y() + workarea.get_height()) * scale_factor - 1;
                    break;
                case dock_location_t::left:
                        insets[struts_position_t::left] = (workarea.get_x() + area) * scale_factor;
                        insets[struts_position_t::left_start] = workarea.get_y() * scale_factor;
                        insets[struts_position_t::left_end] = (workarea.get_y() + workarea.get_height()) * scale_factor -1;
                    break;
                case dock_location_t::right:
                        insets[struts_position_t::right] = (area + screen->get_width()) - workarea.get_x() - workarea.get_width() * scale_factor;
                        insets[struts_position_t::right_start] =  workarea.get_y() * scale_factor;
                        insets[struts_position_t::right_end] = (workarea.get_y() + workarea.get_height()) *   scale_factor - 1;
                    break;
                default:
                    break;
                // clang-format on
        }

        m_strut_set = true;

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

}  // namespace docklight::position