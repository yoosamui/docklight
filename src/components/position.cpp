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

            // g_print("FORM POSITION:\n");
            //  wnck::get_docks();
            int area = config::get_dock_area();
            auto geo = get_monitor_geometry();
            auto wa = get_workarea();
            // area = config::get_dock_area();
            // area += (geo.get_height() - wa.get_height());

            // struts::set_strut(false);
            dock_alignment_t alignment = config::get_dock_alignment();
            Gdk::Rectangle workarea = get_workarea();
            ///////////////// auto const panel = m_window->get_panel();
            int xpos = 0, ypos = 0, center = 0;
            m_strut_set = false;

            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                int width = required_size;
                // if (m_last_width == width) return;

                m_last_width = width;

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
                    // ypos = workarea.get_y() + workarea.get_height() - area;
                    // g_print("area mon: %d  work : %d\n",

                    /*device::monitor::get_geometry().get_height(), workarea.get_height();

                    int y = device::monitor::get_geometry().get_height() - area;
                    // workarea.get_height();
                    ypos = y;
                    ypos = workarea.get_height() - area;

                    // new*/
                    //   int diff = geo.get_height() - wa.get_height() - wa.get_y();
                    //
                    // TODO esta es  la nueva
                    //                    ypos = (wa.get_height() + std::abs(wa.get_y()) - area);

                    // esta es la vieja
                    ypos = workarea.get_y() + workarea.get_height() - area;
                    g_print(
                        "-------------POSITION:\nAREA %d\nypos %d\nwa Height %d\nrequested size: "
                        "%d",
                        area, ypos, workarea.get_height(), required_size);

                    //   50;  //+ std::abs(142 - m_window->get_height());
                    // area;  // - (geo.get_height() - wa.get_height() - wa.get_y());
                    // ypos = geo.get_height() -
                    //(wa.get_y() + wa.get_height());  //   wa.get_height() + diff;  // -
                    //    m_window->get_height();

                    // ypos = //  ..int bottom_pos = wa.get_height() - top_ocuped_area;

                    //                    ypos = diff;  // bottom_pos;

                    // ypos = geo.get_height() -
                    //((geo.get_height() - wa.get_height()) - area - wa.get_y()));
                    //                    ypos -= wa.get_y() - area;
                    // o.get_height() - (geo.get_height() - wa.get_height());  //+ wa.get_y();
                    // - wa.get_y();
                    //   ypos += area;  //+ wa.get_y(); // - area;

                    //     ypos -= area;
                    //                    ypos += area;
                    // - (geo.get_height() - wa.get_height());
                    //  g_print("YPOS %d window docklight h  %d\n", ypos, area);
                    //                    / wa.get_height();
                    //                    ypos = (geo.get_y() - wa.get_y()) - area;
                    // workarea.get_y() + workarea.get_height() - area;
                } else {
                    ypos = workarea.get_y();
                }

                if (config::is_autohide_none()) {
                    // n  struts::set_strut(false);
                }

                //   struts::set_strut(true);
                m_window->resize(width, area);
                m_window->move(xpos, ypos);
                //                struts::set_strut(false);
            } else {
                int height = required_size;
                if (m_last_height == height) return;

                m_last_height = height;
                // if (Panel_render::m_stm.m_decrease_factor > 0) {
                // area -= Panel_render::m_stm.m_decrease_factor;
                //}

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
        // Gdk::Rectangle get_workarea()
        //{
        // Gdk::Rectangle workarea = config::is_autohide_none() ? device::monitor::get_geometry()
        //: device::monitor::get_workarea();

        // return workarea;
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
                //

                if (m_strut_set) return;
                m_strut_set = true;
                long insets[12] = {0};
                bool equal = false;
                GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
                auto gdk_window = gtk_widget_get_window(toplevel);
                if (gdk_window == nullptr) {
                    g_critical("set_strut: gdk_window is null.");
                    return;
                }
                g_print("IN STRUT\n");
                int area = config::get_dock_area();
                auto monitor_geo = get_monitor_geometry();
                auto workarea_geo = device::monitor::get_workarea();
                int screen_width = 2560;  ///* screen->get_width();*/  // 2560;
                int screen_height = 1440;
                // area =(monitor_geo.get_height()-workarea_geo.get_height()) + (area/2);
                auto window_scale_factor = 1;
                /*if (Panel_render::m_stm.m_decrease_factor > 0) {
                    area -= Panel_render::m_stm.m_decrease_factor;
                }*/
                // Gdk::Rectangle workarea =
                // device::monitor::get_current()->get_workarea();
                Gdk::Rectangle workarea = device::monitor::get_geometry();
                auto const location = config::get_dock_location();
                auto const screen = device::display::get_default_screen();

                // clang-format off
            switch (location) {
                case dock_location_t::top:
                    g_print("------------------in T O P - TESTED WORKS+++\n");

                    area +=  workarea_geo.get_y();
                    insets[struts_position_t::top] = workarea.get_y() + area * window_scale_factor;
                    insets[struts_position_t::top_start] = workarea.get_x() *  window_scale_factor;
                    insets[struts_position_t::top_end] = (workarea.get_x() + workarea.get_width()) * window_scale_factor;

                    break;
                case dock_location_t::bottom:

                    g_print("------------------in B O T T O M TESTED WORKS+++\n");

                   area += monitor_geo.get_height()- workarea_geo.get_height() -  workarea_geo.get_y() ;
                    insets[struts_position_t::bottom] =(area + screen->get_height() - workarea.get_y() - workarea.get_height()) * 1;
                    insets[struts_position_t::bottom_start] = workarea.get_y() * 1;
                    insets[struts_position_t::bottom_end] = (workarea.get_y() + workarea.get_height()) * 1;

// TODO BEST WORKING ++++
                   //area += monitor_geo.get_height()- workarea_geo.get_height() -  workarea_geo.get_y() ;
                    //insets[struts_position_t::bottom] =area + screen->get_height() - workarea.get_y() - workarea.get_height();
                    //insets[struts_position_t::bottom_start] = workarea.get_y();
                    //insets[struts_position_t::bottom_end] = workarea.get_y() + workarea.get_height();
                    break;

                case dock_location_t::left:
                    g_print("------------------in LEFT TESTED WORKS+++\n");

                    area +=  workarea_geo.get_x();
                    insets[struts_position_t::left] = workarea.get_x() + area;
                    insets[struts_position_t::left_start] = workarea.get_y();
                    insets[struts_position_t::left_end] = workarea.get_y() + workarea.get_height();

                    break;

                case dock_location_t::right:
                    g_print("------------------in RIGHT TESTED WORKS+++\n");

                    // set the struts  WORK +++++++++
                    area += monitor_geo.get_width()- workarea_geo.get_width() - workarea_geo.get_x();
                    insets[struts_position_t::right] = (area + screen->get_width()) - workarea.get_x() - workarea.get_width() * window_scale_factor;
                    insets[struts_position_t::right_start] = workarea.get_y()* window_scale_factor;
                    insets[struts_position_t::right_end] =(workarea.get_y() + workarea.get_height()) * window_scale_factor - 1;


                    break;

                default:
                    break;
}
                // clang-format on

                //// is equal nothing has changed
                // if (equal) {
                // return;
                //}

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

