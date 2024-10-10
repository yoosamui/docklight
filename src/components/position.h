#pragma once
//  Copyright(c) 2018 - 2024 Juan R.González
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
#include <gtkmm/window.h>

#include "components/device.h"
#include "components/config.h"
#include "utils/struts.h"
// clang-format on

namespace docklight
{
    class PositionManager : public Glib::Object
    {
      public:
        PositionManager(Gtk::Window* window);

        const Gtk::Window* get_window() const;
        Gdk::Rectangle get_workarea() const;
        Gdk::Rectangle get_monitor() const;
        Gdk::Rectangle get_window_geometry() const;

        double get_window_scalling_factor(const guint items_count, const guint area);
        double get_scalling_factor(const guint screen_width, const guint item_size,
                                   const guint size);

        int get_width()
        {
            return m_width;
            // window->get_width();
        }
        void monitor_changed();
        void force_position();
        void reset_position();
        void set_position(guint required_size);
        void set_struts()
        {
            //
            // TODO:
            m_struts.set_struts();
        }

        // void get_window_geometry(gint* x, gint* y, gint* width, gint* height)
        //{
        ////           get_geometry(GdkWindow * window, gint * x, gint * y, gint * width, gint *
        ////           height)
        ////
        //}

        long insets_cache[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        void set_struts3(bool reset)
        {
            typedef enum struts_enum {
                left = 0,
                right = 1,
                top = 2,
                bottom = 3,
                left_start = 4,
                left_end = 5,
                right_start = 6,
                right_end = 7,
                top_start = 8,
                top_end = 9,
                bottom_start = 10,
                bottom_end = 11
            } struts_position_t;

            long insets[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            bool equal = false;
            GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
            auto gdk_window = gtk_widget_get_window(toplevel);
            if (gdk_window == nullptr) {
                g_critical("set_strut: gdk_window is null.");
                return;
            }

            // if (!reset)
            {
                int area = Config()->get_dock_area();
                // if (Panel_render::m_stm.m_decrease_factor > 0) {
                // area -= Panel_render::m_stm.m_decrease_factor;
                //}
                Gdk::Rectangle workarea = get_workarea();
                Gdk::Rectangle monitor = device::monitor::get_geometry();

                // TODO TEST
                // workarea = monitor;

                auto const location = Config()->get_dock_location();
                auto const screen = device::display::get_default_screen();

                float scale_factor = 1;

                // clang-format off
            switch (location) {
                case dock_location_t::top:

                    // set the struts
                    insets[struts_position_t::top] = workarea.get_y() + area;
                    insets[struts_position_t::top_start] = workarea.get_x();
                    insets[struts_position_t::top_end] = workarea.get_x() + workarea.get_width();

                    // compare for equality with the current insets
                    equal = insets_cache[struts_position_t::top] == insets[struts_position_t::top] &&
                            insets_cache[struts_position_t::top_start] ==  insets[struts_position_t::top_start] &&
                            insets_cache[struts_position_t::top_end] == insets[struts_position_t::top_end];

                    if(!equal){
                        // no equal. cache the struts for next call
                        insets_cache[struts_position_t::top] = insets[struts_position_t::top];
                        insets_cache[struts_position_t::top_start] = insets[struts_position_t::top_start];
                        insets_cache[struts_position_t::top_end] = insets[struts_position_t::top_end];
                    }

                    break;
                case dock_location_t::bottom:
               g_print("window y%d area %d\n",m_y, area);
               g_print("strus pos  %d\n", monitor.get_height() - m_y);

                    if(!m_y) return;
                    // set the struts
                    //insets[struts_position_t::bottom] =((area + screen->get_height()) - monitor.get_y() - monitor.get_height())+ get_workarea().get_y()-area;
                    insets[struts_position_t::bottom] =
                        //((area + screen->get_height()) - monitor.get_y() - monitor.get_height())+ get_workarea().get_y()-area;
//                        (area+64) ;//
                 //  215;//  get_workarea().get_y();
                    monitor.get_height() - m_y;
                     //       area +

                    insets[struts_position_t::bottom_start] = monitor.get_x();
                    insets[struts_position_t::bottom_end] = monitor.get_x() + monitor.get_width();

                    //// compare for equality with the current insets
                    //equal = insets_cache[struts_position_t::bottom] == insets[struts_position_t::bottom] &&
                            //insets_cache[struts_position_t::bottom_start] ==  insets[struts_position_t::bottom_start] &&
                            //insets_cache[struts_position_t::bottom_end] == insets[struts_position_t::bottom_end];

                    //if(!equal){
                      ////   no equal. cache the struts for next call
                        //insets_cache[struts_position_t::bottom] = insets[struts_position_t::bottom];
                        //insets_cache[struts_position_t::bottom_start] = insets[struts_position_t::bottom_start];
                        //insets_cache[struts_position_t::bottom_end] = insets[struts_position_t::bottom_end];
                    //}

                    break;
                case dock_location_t::left:

                    // set the struts
                    insets[struts_position_t::left] = workarea.get_x() + area;
                    insets[struts_position_t::left_start] = workarea.get_y();
                    insets[struts_position_t::left_end] = workarea.get_y() + workarea.get_height();

                    // compare for equality with the current insets
                    equal = insets_cache[struts_position_t::left] == insets[struts_position_t::left] &&
                            insets_cache[struts_position_t::left_start] ==  insets[struts_position_t::left_start] &&
                            insets_cache[struts_position_t::left_end] == insets[struts_position_t::left_end];

                    if(!equal){
                        // no equal. cache the struts for next call
                        insets_cache[struts_position_t::left] = insets[struts_position_t::left];
                        insets_cache[struts_position_t::left_start] = insets[struts_position_t::left_start];
                        insets_cache[struts_position_t::left_end] = insets[struts_position_t::left_end];
                    }
                    break;

                case dock_location_t::right:

                    // set the struts
                    insets[struts_position_t::right] = (area + screen->get_width()) - workarea.get_x() - workarea.get_width();
                    insets[struts_position_t::right_start] = workarea.get_y();
                    insets[struts_position_t::right_end] = workarea.get_y() + workarea.get_height();

                    // compare for equality with the current insets
                    equal = insets_cache[struts_position_t::right] == insets[struts_position_t::right] &&
                            insets_cache[struts_position_t::right_start] ==  insets[struts_position_t::right_start] &&
                            insets_cache[struts_position_t::right_end] == insets[struts_position_t::right_end];

                    if(!equal){
                        // no equal. cache the struts for next call
                        insets_cache[struts_position_t::right] = insets[struts_position_t::right];
                        insets_cache[struts_position_t::right_start] = insets[struts_position_t::right_start];
                        insets_cache[struts_position_t::right_end] = insets[struts_position_t::right_end];
                    }

                    break;

                default:
                    break;
            }
                // clang-format on
            }

            // is equal nothing has changed
            if (equal) {
                //   return;
            }
            g_print("set struts................\n");
            g_print(
                "set struts "
                ".................................................................................."
                "....>!\n");
            gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                                gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                                (unsigned char*)&insets, 12);

            gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT", FALSE),
                                gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                                (unsigned char*)&insets, 4);
        }

      private:
        position::Struts m_struts;
        Gtk::Window* m_window;
        guint m_last_required_size = 0;
        guint m_last_ypos = 0;

        int m_lastposx = 0;
        guint m_width = 1;
        guint m_height = 1;
        guint m_y = 0;
        guint m_x = 0;
    };

    Glib::RefPtr<PositionManager> create_position(Gtk::Window* window);
    Glib::RefPtr<PositionManager> Position();

}  // namespace docklight
