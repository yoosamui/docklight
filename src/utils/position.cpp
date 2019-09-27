#include "position.h"
#include "components/config.h"
#include "components/device.h"

DL_NS_BEGIN
namespace position_util
{
    AppWindow* m_window;

    void init(AppWindow* window) { m_window = window; }

    int get_area()
    {
        return (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL)
                   ? m_window->get_height()
                   : m_window->get_width();
    }

    void set_window_position()
    {
        int area = config::get_dock_area();
        Gdk::Rectangle workarea = config::is_autohide_none()
                                      ? device::monitor::get_current()->get_geometry()
                                      : device::monitor::get_current()->get_workarea();

        auto const alignment = config::get_dock_alignment();
        int xpos = 0, ypos = 0, center = 0;

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int width = m_window->get_panel()->get_required_size();
            if (Panel::m_stm.m_decrease_factor > 0) {
                area -= Panel::m_stm.m_decrease_factor;
            }

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
                ypos = workarea.get_y() + workarea.get_height() - area;
            } else {
                ypos = workarea.get_y();
            }

            if (config::is_autohide_none()) {
                position_util::set_strut(false);
            }

            m_window->resize(width, area);
            m_window->move(xpos, ypos);

        } else {
            int height = m_window->get_panel()->get_required_size();
            if (Panel::m_stm.m_decrease_factor > 0) {
                area -= Panel::m_stm.m_decrease_factor;
            }

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
                position_util::set_strut(false);
            }

            m_window->resize(area, height);
            m_window->move(xpos, ypos);
        }
    }

    void hide()
    {
        auto const location = config::get_dock_location();
        int area = position_util::get_area();
        Gdk::Rectangle workarea = config::is_autohide_none()
                                      ? device::monitor::get_current()->get_geometry()
                                      : device::monitor::get_current()->get_workarea();

        int x = 0, y = 0;
        m_window->get_position(x, y);

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            // m_window->resize(m_window->get_width(), 1);

            if (location == dock_location_t::bottom) {
                m_window->move(x, workarea.get_height() + workarea.get_y() - 1);
            } else {
                m_window->move(x, workarea.get_y() - area + 1);
            }
        } else {
            // m_window->resize(1, m_window->get_height());

            if (location == dock_location_t::right) {
                m_window->move(workarea.get_width() + workarea.get_x() - 1, y);
            } else {
                m_window->move(workarea.get_x() - area + 1, y);
            }
        }
    }
    bool is_visible()
    {
        int x = 0, y = 0;
        int area = get_area();
        auto const location = config::get_dock_location();
        Gdk::Rectangle workarea = config::is_autohide_none()
                                      ? device::monitor::get_current()->get_geometry()
                                      : device::monitor::get_current()->get_workarea();

        m_window->get_position(x, y);

        switch (location) {
            case dock_location_t::top:
                return y == workarea.get_y();
            case dock_location_t::bottom:
                return (y + area) == (workarea.get_y() + workarea.get_height());
            case dock_location_t::left:
                return x == workarea.get_x();
            case dock_location_t::right:
                return (x + area) == (workarea.get_x() + workarea.get_width());
            default:
                return false;
        }
    }
    const Gdk::Rectangle get_appwindow_geometry()
    {
        Gdk::Rectangle result;
        int x, y;
        m_window->get_position(x, y);

        result.set_x(x);
        result.set_y(y);
        result.set_width(m_window->get_width());
        result.set_height(m_window->get_height());

        return result;
    }

    void get_center_screen_position(int targetwidth, int targetheight, int& posx, int& posy)
    {
        Gdk::Rectangle workarea = device::monitor::get_current()->get_workarea();

        int monitorWidth = workarea.get_width();
        int monitorHeight = workarea.get_height();

        int monitorcenterWidth = monitorWidth / 2;
        int monitorcenterHeight = monitorHeight / 2;

        posx = monitorcenterWidth - (targetwidth / 2);
        posy = monitorcenterHeight - (targetheight / 2);
    }

    void reset_strut() { set_strut(true); }
    void set_strut(bool reset)
    {
        long insets[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
        auto gdk_window = gtk_widget_get_window(toplevel);
        if (gdk_window == NULL) {
            g_critical("set_strut: gdk_window is null.");
            return;
        }

        if (!reset) {
            int area = config::get_dock_area();
            if (Panel::m_stm.m_decrease_factor > 0) {
                area -= Panel::m_stm.m_decrease_factor;
            }
            // Gdk::Rectangle workarea = device::monitor::get_current()->get_workarea();
            Gdk::Rectangle workarea = device::monitor::get_current()->get_geometry();
            auto const location = config::get_dock_location();
            auto const screen = device::display::get_default_screen();

            switch (location) {
                case dock_location_t::top:
                    insets[struts_position_t::top] = workarea.get_y() + area;
                    insets[struts_position_t::top_start] = workarea.get_x();
                    insets[struts_position_t::top_end] = workarea.get_x() + workarea.get_width();

                    break;
                case dock_location_t::bottom:
                    insets[struts_position_t::bottom] =
                        (area + screen->get_height()) - workarea.get_y() - workarea.get_height();
                    insets[struts_position_t::bottom_start] = workarea.get_x();
                    insets[struts_position_t::bottom_end] = workarea.get_x() + workarea.get_width();

                    break;
                case dock_location_t::left:
                    insets[struts_position_t::left] = workarea.get_x() + area;
                    insets[struts_position_t::left_start] = workarea.get_y();
                    insets[struts_position_t::left_end] = workarea.get_y() + workarea.get_height();
                    break;

                case dock_location_t::right:
                    insets[struts_position_t::right] =
                        (area + screen->get_width()) - workarea.get_x() - workarea.get_width();
                    insets[struts_position_t::right_start] = workarea.get_y();
                    insets[struts_position_t::right_end] = workarea.get_y() + workarea.get_height();

                    break;
                default:
                    return;
            }
        }

        gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                            gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                            (unsigned char*)&insets, 12);

        gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT", FALSE),
                            gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                            (unsigned char*)&insets, 4);
    }

}  // namespace position_util
DL_NS_END

