// clang-format off
#include "components/position.h"
#include "components/device.h"
// clang-format on

namespace docklight
{

    namespace position
    {
        Gtk::Window* m_window = nullptr;

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

        void set_window_position()
        {
            int area = config::get_dock_area();
            dock_alignment_t alignment = config::get_dock_alignment();
            Gdk::Rectangle workarea = get_workarea();
            ///////////////// auto const panel = m_window->get_panel();
            int xpos = 0, ypos = 0, center = 0;

            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                int width = 1000;  /////panel->get_required_size();//TODO remove after testing
                if (width > workarea.get_width()) {
                    width = workarea.get_width();
                }

                switch (alignment) {
                    case dock_alignment_t::start:
                        xpos = workarea.get_x();
                        g_message("1111111111111111");
                        break;

                    case dock_alignment_t::end:
                        xpos = workarea.get_x() + workarea.get_width() - width;
                        g_message("2222222222222222222222");
                        break;

                    case dock_alignment_t::center:
                        center = workarea.get_width() / 2 - width / 2;
                        xpos = workarea.get_x() + center;
                        g_message("3333333333333333333333");
                        break;

                    default:  // fill
                        xpos = workarea.get_x();
                        width = workarea.get_width();
                        g_message("FILLLLLLLLLLLLLLLLLL");
                }

                if (config::get_dock_location() == dock_location_t::bottom) {
                    ypos = workarea.get_y() + workarea.get_height() - area;
                } else {
                    ypos = workarea.get_y();
                }

                if (config::is_autohide_none()) {
                    struts::set_strut(false);
                    g_message("SET STRUT");
                }

                g_message("AREA %d w:%d h%d\n", area, width, area);
                m_window->resize(width, area);
                m_window->move(xpos, ypos);
            } else {
                int height = 1000;  // panel->get_required_size(); //TODO remove after testing
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
                    xpos = workarea.get_x() + workarea.get_width() - area;
                } else {
                    xpos = workarea.get_x();
                }

                if (config::is_autohide_none()) {
                    struts::set_strut(false);
                }

                m_window->resize(area, height);
                m_window->move(xpos, ypos);
            }
        }

        Gdk::Rectangle get_workarea()
        {
            Gdk::Rectangle workarea = config::is_autohide_none() ? device::monitor::get_geometry()
                                                                 : device::monitor::get_workarea();

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
                long insets[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                bool equal = false;
                GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
                auto gdk_window = gtk_widget_get_window(toplevel);
                if (gdk_window == nullptr) {
                    g_critical("set_strut: gdk_window is null.");
                    return;
                }

                if (!reset) {
                    int area = config::get_dock_area();
                    /*if (Panel_render::m_stm.m_decrease_factor > 0) {
                        area -= Panel_render::m_stm.m_decrease_factor;
                    }*/
                    // Gdk::Rectangle workarea = device::monitor::get_current()->get_workarea();
                    Gdk::Rectangle workarea = device::monitor::get_geometry();
                    auto const location = config::get_dock_location();
                    auto const screen = device::display::get_default_screen();

                    // clang-format off
            switch (location) {
                case dock_location_t::top:

                    // set the struts
                    insets[struts_position_t::top] = workarea.get_y() + area;
                    insets[struts_position_t::top_start] = workarea.get_x();
                    insets[struts_position_t::top_end] = workarea.get_x() + workarea.get_width();

                    break;
                case dock_location_t::bottom:

                    // set the struts
                    insets[struts_position_t::bottom] =(area + screen->get_height()) - workarea.get_y() - workarea.get_height();
                    insets[struts_position_t::bottom_start] = workarea.get_x();
                    insets[struts_position_t::bottom_end] = workarea.get_x() + workarea.get_width();

                    break;
                case dock_location_t::left:

                    // set the struts
                    insets[struts_position_t::left] = workarea.get_x() + area;
                    insets[struts_position_t::left_start] = workarea.get_y();
                    insets[struts_position_t::left_end] = workarea.get_y() + workarea.get_height();

                    break;

                case dock_location_t::right:

                    // set the struts
                    insets[struts_position_t::right] = (area + screen->get_width()) - workarea.get_x() - workarea.get_width();
                    insets[struts_position_t::right_start] = workarea.get_y();
                    insets[struts_position_t::right_end] = workarea.get_y() + workarea.get_height();


                    break;

                default:
                    break;
            }
                    // clang-format on
                }

                // is equal nothing has changed
                if (equal) {
                    return;
                }
                g_print("set struts................\n");
                gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                                    gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                                    (unsigned char*)&insets, 12);

                gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT", FALSE),
                                    gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                                    (unsigned char*)&insets, 4);
            }

        }  // namespace struts
    }      // namespace position

}  // namespace docklight

