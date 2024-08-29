//  Copyright (c) 2018-2024 Juan R. González
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
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <iostream>

#include "ExplodesWindow.h"
#include "utils/system.h"
#include "translations.h"
// clang-format on

namespace docklight
{

    ExplodesWindow::ExplodesWindow()
    {
        set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
        set_can_focus(false);

        auto filename = "data/images/explodes.svg";

        try {
            m_image = Gdk::Pixbuf::create_from_file(filename);
            m_size = m_image->get_width();
            m_frames = std::floor(m_image->get_height() / m_size);
        } catch (const std::exception& ex) {
            g_critical("Exception 'ExplodesWindow'. :%s", ex.what());
        } catch (...) {
            std::exception_ptr p = std::current_exception();
            auto message =
                "Exception 'ExplodesWindow'. Unable to load the desired image: %s, type: %s";
            g_critical(message, filename, p ? p.__cxa_exception_type()->name() : "unknown type");
        }

        set_size_request(m_size, m_size);
    }

    ExplodesWindow::~ExplodesWindow()
    {
        g_print(MSG_FREE_OBJECT, "ExplodesWindow");
        g_print("\n");
    }

    void ExplodesWindow::ConnectSignal(bool connect)
    {
        if (connect) {
            m_timeout_draw = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &ExplodesWindow::on_timeout_draw), DF_EXPLODES_FRAMERATE);
        } else {
            m_timeout_draw.disconnect();
        }
    }

    bool ExplodesWindow::on_timeout_draw()
    {
        m_frame_time = g_get_real_time();
        if (m_frame_time - m_start_time <= DF_EXPLODES_TIMEMAX) {
            Gtk::Widget::queue_draw();
            return true;
        }

        hide();
        ConnectSignal(false);
        return true;
    }
    void ExplodesWindow::show_at(int x, int y)
    {
        m_start_time = g_get_real_time();
        m_frame_time = m_start_time;
        ConnectSignal(true);

        show();
        move(x - (m_size / 2), y - (m_size / 2));
    }

    bool ExplodesWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        // Replace destination layer (bounded)
        cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);

        int ypos = -m_size * (int)(m_frames * (m_frame_time - m_start_time) / DF_EXPLODES_TIMEMAX);
        Gdk::Cairo::set_source_pixbuf(cr, m_image, 0, ypos);
        cr->paint();

        return false;
    }
}  // namespace docklight
