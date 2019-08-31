#include "position.h"
#include "components/config.h"
#include "components/device.h"

DL_NS_BEGIN
namespace position_util
{
    AppWindow* m_window;

    void init(AppWindow* window) { m_window = window; }

    void set_window_position()
    {
        int area = config::get_dock_area();
        Gdk::Rectangle workarea = device::monitor::get_current()->get_workarea();
        auto const alignment = config::get_dock_alignment();
        int xpos = 0, ypos = 0, center = 0;

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int width = m_window->get_panel()->get_required_size();
            if (width > workarea.get_width()) {
                width = workarea.get_width();
            }

            switch (alignment) {
                case dock_alignment_t::start:
                    ypos = workarea.get_y();
                    xpos = workarea.get_x();
                    break;

                case dock_alignment_t::end:
                    ypos = workarea.get_y();
                    xpos = workarea.get_x() + workarea.get_width() - width;
                    break;

                case dock_alignment_t::center:
                    center = workarea.get_width() / 2 - width / 2;
                    ypos = workarea.get_y();
                    xpos = workarea.get_x() + center;
                    break;

                default:  // fill
                    ypos = workarea.get_y();
                    xpos = workarea.get_x();
                    width = workarea.get_width();
            }

            if (config::get_dock_location() == dock_location_t::bottom) {
                ypos = workarea.get_y() + workarea.get_height() - area;
            }

            m_window->resize(width, area);
            m_window->move(xpos, ypos);

        } else {
            int height = m_window->get_panel()->get_required_size();
            if (height > workarea.get_height()) {
                height = workarea.get_height();
            }

            switch (alignment) {
                case dock_alignment_t::start:
                    ypos = workarea.get_y();
                    xpos = workarea.get_x();
                    break;

                case dock_alignment_t::end:
                    ypos = workarea.get_y() + workarea.get_height() - height;
                    xpos = workarea.get_x();
                    break;

                case dock_alignment_t::center:
                    center = workarea.get_height() / 2 - height / 2;
                    ypos = workarea.get_y() + center;
                    xpos = workarea.get_x();
                    break;

                default:  // fill
                    ypos = workarea.get_y();
                    xpos = workarea.get_x();
                    height = workarea.get_height();
                    break;
            }

            if (config::get_dock_location() == dock_location_t::right) {
                xpos = workarea.get_x() + workarea.get_width() - area;
            }

            m_window->resize(area, height);
            m_window->move(xpos, ypos);
        }
    }

    void hide()
    {
        auto const location = config::get_dock_location();
        Gdk::Rectangle workarea = device::monitor::get_current()->get_workarea();

        int x = 0, y = 0;
        m_window->get_position(x, y);

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            m_window->resize(m_window->get_width(), 2);

            if (location == dock_location_t::bottom) {
                m_window->move(x, workarea.get_height() + workarea.get_y() - 2);
            } else {
                m_window->move(x, workarea.get_y());
            }
        } else {
            m_window->resize(2, m_window->get_height());

            if (location == dock_location_t::right) {
                m_window->move(workarea.get_width() + workarea.get_x() - 2, y);
            } else {
                m_window->move(workarea.get_x(), y);
            }
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
}  // namespace position_util
DL_NS_END

