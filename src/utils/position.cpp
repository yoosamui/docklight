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
        Gdk::Rectangle workarea =
            device::monitor::get_current()->get_workarea();

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int width = m_window->get_panel()->get_required_size();
            if (width > workarea.get_width()) {
                width = workarea.get_width();
            }

            m_window->resize(width, area);

            int center = workarea.get_width() / 2 - width / 2;
            int ypos = workarea.get_y();
            int xpos = workarea.get_x() + center;

            if (config::get_dock_location() == dock_location_t::bottom) {
                ypos = workarea.get_y() + workarea.get_height() - area;
            }

            m_window->move(xpos, ypos);

        } else {
            int height = m_window->get_panel()->get_required_size();
            if (height > workarea.get_height()) {
                height = workarea.get_height();
            }

            m_window->resize(area, height);

            int center = workarea.get_height() / 2 - height / 2;
            int ypos = workarea.get_y() + center;
            int xpos = workarea.get_x();

            if (config::get_dock_location() == dock_location_t::right) {
                xpos = workarea.get_x() + workarea.get_width() - area;
            }

            m_window->move(xpos, ypos);
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

}  // namespace position_util
DL_NS_END

