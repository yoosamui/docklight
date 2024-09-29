// clang-format off
#include "components/position.h"
#include "components/device.h"
// clang-format on
#include <gtkmm.h>
namespace docklight
{

    namespace position
    {
        Gtk::Window* m_window = nullptr;
        int m_last_width = 0;
        int m_last_height = 0;
        bool m_strut_set = false;

        void init(Gtk::Window& window)
        {
            // direct access to AppWindow;
            m_window = &(window);
        }

        const Gtk::Window* get_window()
        {
            return m_window;
        }

        Gdk::Rectangle get_background_region()
        {
            Gdk::Rectangle result(0, 0, m_window->get_width(), m_window->get_height());
            return result;
        }

        int m_cwindow_width = 0;
        void set_window_position(guint required_size = 1000)

        {
            gint w_width = required_size;

            if (m_cwindow_width == w_width) return;
            m_cwindow_width = w_width;

            int area = config::get_dock_area();

            dock_alignment_t alignment = config::get_dock_alignment();
            Gdk::Rectangle workarea_geo = get_workarea();

            int xpos = 0, ypos = 0, center = 0;
            m_strut_set = false;

            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                int width = required_size;
                // if (m_last_width == width) return;

                m_last_width = width;

                if (width > workarea_geo.get_width()) {
                    width = workarea_geo.get_width();
                }

                switch (alignment) {
                    case dock_alignment_t::start:
                        xpos = workarea_geo.get_x();
                        break;

                    case dock_alignment_t::end:
                        xpos = workarea_geo.get_x() + workarea_geo.get_width() - width;
                        break;

                    case dock_alignment_t::center:
                        center = workarea_geo.get_width() / 2 - width / 2;
                        xpos = workarea_geo.get_x() + center;
                        break;

                    default:  // fill
                        xpos = workarea_geo.get_x();
                        width = workarea_geo.get_width();
                }

                if (config::get_dock_location() == dock_location_t::bottom) {
                    ypos = workarea_geo.get_y() + workarea_geo.get_height() - area;
                } else {
                    ypos = workarea_geo.get_y();
                }

                if (config::is_autohide_none()) {
                    //  struts::set_strut(false);
                }

                m_window->resize(width, area);
                m_window->move(xpos, ypos);
            } else {
                int height = required_size;
                if (m_last_height == height) return;

                m_last_height = height;
                // if (Panel_render::m_stm.m_decrease_factor > 0) {
                // area -= Panel_render::m_stm.m_decrease_factor;
                //}

                if (height > workarea_geo.get_height()) {
                    height = workarea_geo.get_height();
                }

                switch (alignment) {
                    case dock_alignment_t::start:
                        ypos = workarea_geo.get_y();
                        break;

                    case dock_alignment_t::end:
                        ypos = workarea_geo.get_y() + workarea_geo.get_height() - height;
                        break;

                    case dock_alignment_t::center:
                        center = workarea_geo.get_height() / 2 - height / 2;
                        ypos = workarea_geo.get_y() + center;
                        break;

                    default:  // fill
                        ypos = workarea_geo.get_y();
                        height = workarea_geo.get_height();
                        break;
                }

                if (config::get_dock_location() == dock_location_t::right) {
                    // xpos = workarea_geo.get_x() + workarea.get_width() - area;
                    xpos = workarea_geo.get_width() - area;
                } else {
                    xpos = workarea_geo.get_x();
                }

                if (config::is_autohide_none()) {
                    //    struts::set_strut(false);
                }

                m_window->resize(area, height);
                m_window->move(xpos, ypos);
            }
            //   g_print("Setut set!!!!\n");
            struts::set_strut(false);
        }

        void get_docks()
        {
            wnck::get_docks();
        }

        Gdk::Rectangle get_monitor_geometry()
        {
            Gdk::Rectangle size = device::monitor::get_geometry();
            return size;
        }

        // update_client_geometry(int& x, int& y, int& width, int& height) {}
        // Gdk::Rectangle get_workarea_geo()
        //{
        // Gdk::Rectangle workarea_geo = config::is_autohide_none() ?
        // device::monitor::get_geometry()
        //: device::monitor::get_workarea_geo();

        // return workarea_geo;
        //}

        Gdk::Rectangle get_workarea()
        {
            Gdk::Rectangle workarea = device::monitor::get_workarea();

            return workarea;
        }

        namespace struts
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

            void set_strut(bool reset)
            {
                if (m_strut_set) return;
                m_strut_set = true;

                long insets[12] = {0};

                g_print("IN STRUT\n");
                auto const screen = device::display::get_default_screen();

                int area = config::get_dock_area();

                auto monitor_geo = get_monitor_geometry();
                auto workarea_geo = device::monitor::get_workarea();

                int screen_width = screen->get_width();
                int screen_height = 1440;  // screen->get_height();
                auto window_scale_factor = 1;
                /*if (Panel_render::m_stm.m_decrease_factor > 0) {
                    area -= Panel_render::m_stm.m_decrease_factor;
                }*/
                auto const location = config::get_dock_location();

                switch (location) {
                    case dock_location_t::top:
                        g_print("------------------in T O P - TESTED WORKS+++QA PASS\n");

                        // clang-format off
                      //  area += workarea_geo.get_y();
                        insets[struts_position_t::top] = workarea_geo.get_y() + area * window_scale_factor;
                        insets[struts_position_t::top_start] = workarea_geo.get_x() * window_scale_factor;
                        insets[struts_position_t::top_end] = (workarea_geo.get_x() + workarea_geo.get_width()) * window_scale_factor;

                        // clang-format on
                        break;
                    case dock_location_t::bottom:

                        g_print(
                            "------------------in B O T T O M TESTED WORKS+++AREA: %d QA PASS\n",
                            area);
                        // clang-format off
//                        area += monitor_geo.get_height() - workarea_geo.get_height() ;//-  workarea_geo.get_y();

                        insets[struts_position_t::bottom] =  (area + screen->get_height() - workarea_geo.get_y() -   workarea_geo.get_height()) *     1;
                        insets[struts_position_t::bottom_start] = workarea_geo.get_y() * 1;
                        insets[struts_position_t::bottom_end] = (workarea_geo.get_y() + workarea_geo.get_height()) * 1;

                        // clang-format on

                        // TODO BEST WORKING ++++
                        // area += monitor_geo.get_height()- workarea_geo.get_height() -
                        // workarea_geo.get_y() ; insets[struts_position_t::bottom] =area +
                        // screen->get_height() - workarea.get_y() - workarea.get_height();
                        // insets[struts_position_t::bottom_start] = workarea.get_y();
                        // insets[struts_position_t::bottom_end] = workarea.get_y() +
                        // workarea.get_height();
                        break;

                    case dock_location_t::left:
                        g_print("------------------in LEFT  TESTED WORKS+++AREA: %d QA PASS\n",
                                area);

                        // clang-format off
                        //                        area += workarea_geo.get_x();
                        insets[struts_position_t::left] = (workarea_geo.get_x() + area) * 1;
                        insets[struts_position_t::left_start] = workarea_geo.get_y() * 1;
                        insets[struts_position_t::left_end] = (workarea_geo.get_y() + workarea_geo.get_height()) * 1;
                        // clang-format on

                        break;

                    case dock_location_t::right:
                        g_print("------------------in RIGHT TESTED WORKS+++AREA: %d QA PASS\n",
                                area);

                        // clang-format off
                        // set the struts  WORK +++++++++
//                        area += monitor_geo.get_width() - workarea_geo.get_width() -                                workarea_geo.get_x();
                        insets[struts_position_t::right] = (area + screen->get_width()) - workarea_geo.get_x() - workarea_geo.get_width() * window_scale_factor;
                        insets[struts_position_t::right_start] =  workarea_geo.get_y() * window_scale_factor;
                        insets[struts_position_t::right_end] = (workarea_geo.get_y() + workarea_geo.get_height()) *   window_scale_factor - 1;

                        // clang-format on
                        break;

                    default:
                        break;
                }

                GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
                auto gdk_window = gtk_widget_get_window(toplevel);
                if (!gdk_window) {
                    g_critical("set_strut: gdk_window is null.");
                    return;
                }
                gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                                    gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                                    (unsigned char*)&insets, 12);

                gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT", FALSE),
                                    gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                                    (unsigned char*)&insets, 4);
            }
        }  // namespace struts

    }  // namespace position

}  // namespace docklight

