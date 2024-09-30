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

    void Struts::set_struts(bool create)
    {
        if (m_strut_set && create) return;
        m_strut_set = true;

        long insets[12] = {0};

        if (create) {
            auto const screen = device::display::get_default_screen();
            auto const location = config::get_dock_location();
            auto area = config::get_dock_area();
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

            g_message("Struts request has bin set.");
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
}  // namespace docklight::position
