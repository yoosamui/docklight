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
        set_skip_taskbar_hint(true);
        set_skip_pager_hint(true);

        // TODO GLib-GObject-CRITICAL **: 09:05:26.447: g_object_ref: assertion
        // 'G_IS_OBJECT (object)' failedthis cause a Glib Critical assertion
        // set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
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

        set_resizable(false);
        set_can_focus(false);

        m_size = m_image->get_width();
        set_size_request(m_size, m_size);
    }

    ExplodesWindow::~ExplodesWindow()
    {
        connect_signal(false);

        g_print(MSG_FREE_OBJECT, "ExplodesWindow");
        g_print("\n");
    }

    void ExplodesWindow::connect_signal(bool connect)
    {
        if (connect) {
            m_sigc_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &ExplodesWindow::on_timeout_draw), DF_EXPLODES_FRAMERATE);
        } else {
            m_sigc_connection.disconnect();
        }
    }

    bool ExplodesWindow::on_timeout_draw()

    {
        m_frame_time = g_get_real_time();
        if (m_frame_time - m_start_time <= DF_EXPLODES_TIMEMAX) {
            //  hide();  cause  GLib-GObject-CRITICAL **: 09:33:13.256: g_object_unref: assertion
            //  'G_IS_OBJECT (object)' failed
            // TODO Work around
            /*
             Asks to keep window above, so that it stays on top.
             Note that you shouldn’t assume the window is definitely
             above afterward, because other entities (e.g. the user or
             [window manager][gtk-X11-arch]) could not keep it above,
             and not all window managers support keeping windows above.
             But normally the window will end kept above.

             Just don’t write code that crashes if not.
             */
            set_keep_above();

            Gtk::Widget::queue_draw();
            return true;
        }

        //  hide();  cause  GLib-GObject-CRITICAL **: 09:33:13.256: g_object_unref: assertion
        //  'G_IS_OBJECT (object)' failed
        // TODO Work around
        /*
         Asks to keep window below, so that it stays on top.
         Note that you shouldn’t assume the window is definitely
         below afterward, because other entities (e.g. the user or
         [window manager][gtk-X11-arch]) could not keep it below,
         and not all window managers support keeping windows below.
         But normally the window will end kept below.

         Just don’t write code that crashes if not.
         */
        set_keep_below();
        return true;
    }
    void ExplodesWindow::show_at(int x, int y)
    {
        m_start_time = g_get_real_time();
        m_frame_time = m_start_time;

        connect_signal(true);

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
