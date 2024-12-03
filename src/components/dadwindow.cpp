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
#include "components/dadwindow.h"
// clang-format on

namespace docklight
{

    DADWindow::DADWindow(const Glib::RefPtr<Configuration> &config,
                         const Glib::RefPtr<PositionManager> &position,
                         Glib::RefPtr<Gdk::Pixbuf> icon)
        : Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
    {
        GdkScreen *screen;
        GdkVisual *visual;

        gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
        screen = gdk_screen_get_default();
        visual = gdk_screen_get_rgba_visual(screen);

        if (visual != NULL && gdk_screen_is_composited(screen)) {
            gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
        }

        set_resizable(true);
        set_skip_taskbar_hint(true);
        set_skip_pager_hint(true);
        set_keep_above(true);
        set_keep_above(true);

        m_icon = icon;
        m_config = config;
        m_position = position;

        m_size = m_config->get_icon_size();
        resize(m_size, m_size);
    }

    /**
     * Hide the window on destroy
     */
    DADWindow::~DADWindow()
    {
        hide();
        g_message("release DADWindow\n");
    }

    void DADWindow::close_now()
    {
        close();
    }

    void DADWindow::move_at(int x, int y)
    {
        // int xx, yy;

        // auto area = m_config->get_dock_area();
        // if (m_config->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        // xx = m_position->get_x() + dockitem_index * area;
        // yy = m_position->get_y();

        //} else {
        // yy = m_position->get_y() + dockitem_index * area;
        // xx = m_position->get_x();
        //}

        // int margin = m_config->get_dock_area_margin() / 2;

        x -= m_x;
        y -= m_y;

        // x -= (get_width() / 2) - margin;
        // y -= (get_height() / 2) - margin;
        // int max = std::max(m_x, x);
        //  int min = std::min(m_x, x);

        //  x = (x - m_x);  // x - m_x;  // x_x  //(get_width() / 2) - margin;
        //  x += m_x - (m_size / 2);
        //  y += m_y - (m_size / 2);
        // std::abs(y - m_y);  // x - m_x;  // x_x  //(get_width() / 2) - margin;

        //    max = std::max(m_y, y);
        //    min = std::min(m_y, y);
        //  y += max - min;  // x_x  //(get_width() / 2) - margin;

        //    g_print("MOVE %d %d DIFF \n", m_x, m_y);

        if (x && y) move(x, y);
        //   m_x = x;
        //   m_y = y;

        /// int margin = m_config->get_dock_area_margin() / 2;
        // x -= m_x;  // / 2) - m_x - x;
        //   y -= m_y;  // / 2) - m_y - y;

        //    x -= (m_size / 2) - margin;
        //    y -= (m_size / 2) - margin;

        //        move(x, y);
        // m_x = x;
        // m_y = y;
    }

    void DADWindow::show_at(int dockitem_index)
    {
        int xx, yy;

        auto area = m_config->get_dock_area();
        int margin = m_config->get_dock_area_margin() / 2;

        int mx = 0;
        int my = 0;
        system::get_mouse_position(mx, my);

        if (m_config->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            xx = m_position->get_x() + dockitem_index * area;
            yy = m_position->get_y();

            if (!m_x) m_x = mx - xx - margin;
            if (!m_y) m_y = my - (m_position->get_y() + margin);

        } else {
            yy = m_position->get_y() + dockitem_index * area;
            xx = m_position->get_x();

            if (!m_x) m_x = mx - (m_position->get_x() + margin);
            if (!m_y) m_y = my - yy - margin;
        }

        int x = xx + margin;
        int y = yy + margin;

        if (x && y) move(x, y);

        // m_x = m_y = 2;
        //        g_print("SATRT MOVE %d %d diff %d x %d\n", x, y, m_x, m_y);
        show_all();
    }

    bool DADWindow::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
    {
        cr->set_source_rgba(1, 1, 1, 0);
        Gdk::Cairo::set_source_pixbuf(cr, m_icon, 0, 0);
        cr->paint();

        return true;
    }

}  // namespace docklight

