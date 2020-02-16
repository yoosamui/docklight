#include "position.h"
#include "components/config.h"
#include "components/device.h"

DL_NS_BEGIN
namespace position_util
{
    AppWindow* m_window;
    long insets_cache[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    void init(AppWindow* window) { m_window = window; }

    int get_area()
    {
        return (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL)
                   ? m_window->get_height()
                   : m_window->get_width();
    }

    Gdk::Rectangle get_workarea()
    {
        Gdk::Rectangle workarea = config::is_autohide_none() ? device::monitor::get_geometry()
                                                             : device::monitor::get_workarea();

        // auto const alignment = config::get_dock_alignment();
        // return alignment == dock_alignment_t::fill ?
        // device::monitor::get_current()->get_geometry()
        //    : device::monitor::get_current()->get_workarea();

        return workarea;
    }

    int get_workarea_max_size()
    {
        auto const workarea = get_workarea();
        return config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL
                   ? workarea.get_width()
                   : workarea.get_height();
    }

    void set_window_position()
    {
        int area = config::get_dock_area();
        auto const workarea = get_workarea();
        auto const alignment = config::get_dock_alignment();
        auto const panel = m_window->get_panel();
        int xpos = 0, ypos = 0, center = 0;

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int width = panel->get_required_size();
            if (Panel_render::m_stm.m_decrease_factor > 0) {
                area -= Panel_render::m_stm.m_decrease_factor;
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
            int height = panel->get_required_size();
            if (Panel_render::m_stm.m_decrease_factor > 0) {
                area -= Panel_render::m_stm.m_decrease_factor;
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
        Gdk::Rectangle workarea = get_workarea();

        int anchor_margin = config::get_anchor_margin();

        int x = 0, y = 0;
        m_window->get_position(x, y);

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            //            m_window->resize(m_window->get_width(), anchor_margin);
            if (location == dock_location_t::bottom) {
                m_window->move(x, workarea.get_height() + (workarea.get_y() - anchor_margin));
            } else {
                m_window->move(x, workarea.get_y());
            }
        } else {
            // m_window->resize(anchor_margin, m_window->get_height());

            //            m_window->set_size_request(anchor_margin, m_window->get_height());
            // m_window->set_opacity(0.0);

            if (location == dock_location_t::right) {
                m_window->move(workarea.get_width() + (workarea.get_x() - anchor_margin), y);
            } else {
                m_window->move(workarea.get_x() - area + anchor_margin, y);
            }
        }
    }
    void hide_full()
    {
        auto const location = config::get_dock_location();
        Gdk::Rectangle workarea = get_workarea();

        int x = 0, y = 0;
        m_window->get_position(x, y);

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            if (location == dock_location_t::bottom) {
                m_window->move(x, workarea.get_height() + workarea.get_y() + 1);
            } else {
                m_window->move(x, workarea.get_y() - 1);
            }
        } else {
            if (location == dock_location_t::right) {
                m_window->move(workarea.get_width() + workarea.get_x() + 1, y);
            } else {
                m_window->move(workarea.get_x() - 1, y);
            }
        }
    }
    bool is_visible()
    {
        int x = 0, y = 0;
        int area = get_area();
        auto const location = config::get_dock_location();
        Gdk::Rectangle workarea = get_workarea();

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
        Gdk::Rectangle workarea = device::monitor::get_geometry();

        int monitorWidth = workarea.get_width();
        int monitorHeight = workarea.get_height();

        int monitorcenterWidth = monitorWidth / 2;
        int monitorcenterHeight = monitorHeight / 2;

        posx = monitorcenterWidth - (targetwidth / 2);
        posy = monitorcenterHeight - (targetheight / 2);
    }

    bool get_center_position(int index, int& x, int& y, int width, int height)
    {
        if (index < 0 || index > (int)AppUpdater::m_dockitems.size()) {
            return false;
        }

        auto const item = AppUpdater::m_dockitems[index];
        auto const rect = get_appwindow_geometry();
        auto const workarea = get_workarea();
        auto const location = config::get_dock_location();
        x = rect.get_x();
        y = rect.get_y();

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int center = (item->get_width() / 2) - (width / 2);
            x += item->get_x() + center;
            if (location == dock_location_t::bottom) {
                y -= height;
            } else {
                y += rect.get_height();
            }

            if (x + width > workarea.get_width() + workarea.get_x()) {
                x = workarea.get_width() + workarea.get_x() - width;
                return true;
            }

            if (x < workarea.get_x()) {
                x = workarea.get_x();
            }
        } else {
            int center = (item->get_height() / 2) - (height / 2);
            y += item->get_y() + center;

            if (location == dock_location_t::right) {
                x -= width;
            } else {
                x += rect.get_width();
            }

            if (y + height > workarea.get_height() + workarea.get_y()) {
                y = workarea.get_height() + workarea.get_y() - height;
            }

            if (y < workarea.get_y()) {
                y = workarea.get_y();
            }
        }

        return true;
    }

    void reset_strut() { set_strut(true); }
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
            if (Panel_render::m_stm.m_decrease_factor > 0) {
                area -= Panel_render::m_stm.m_decrease_factor;
            }
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

                    // set the struts
                    insets[struts_position_t::bottom] =(area + screen->get_height()) - workarea.get_y() - workarea.get_height();
                    insets[struts_position_t::bottom_start] = workarea.get_x();
                    insets[struts_position_t::bottom_end] = workarea.get_x() + workarea.get_width();

                    // compare for equality with the current insets
                    equal = insets_cache[struts_position_t::bottom] == insets[struts_position_t::bottom] &&
                            insets_cache[struts_position_t::bottom_start] ==  insets[struts_position_t::bottom_start] &&
                            insets_cache[struts_position_t::bottom_end] == insets[struts_position_t::bottom_end];

                    if(!equal){
                        // no equal. cache the struts for next call
                        insets_cache[struts_position_t::bottom] = insets[struts_position_t::bottom];
                        insets_cache[struts_position_t::bottom_start] = insets[struts_position_t::bottom_start];
                        insets_cache[struts_position_t::bottom_end] = insets[struts_position_t::bottom_end];
                    }

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

}  // namespace position_util
DL_NS_END

