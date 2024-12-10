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

#include <gdkmm/general.h>  // set_source_pixbuf()
#include "testwindow.h"
#include "utils/system.h"
#include "translations.h"
// clang-format on

namespace docklight
{

    TestWindow::TestWindow()
    //: Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
    {
        /*GdkScreen* screen;
        GdkVisual* visual;

        gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
        screen = gdk_screen_get_default();
        visual = gdk_screen_get_rgba_visual(screen);

        if (visual != NULL && gdk_screen_is_composited(screen)) {
            gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
        }*/
        set_decorated(true);

        set_resizable(true);
        // set_skip_taskbar_hint(true);
        // set_skip_pager_hint(true);
        set_keep_above(true);

        set_size_request(400, 400);
        // set_resizable(true);
        // set_skip_taskbar_hint(true);
        // set_skip_pager_hint(true);
        // set_keep_above(true);

        // set_skip_taskbar_hint(true);
        // set_skip_pager_hint(true);
        // set_resizable(false);
        // set_can_focus(false);
        // set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

        // set_keep_above(true);

        //  m_testdraw = new TestDraw();
        //    add(m_t2);

        show_all();

        // m_widget = GTK_WIDGET(m_t2.gobj());
        m_widget = GTK_WIDGET(this->gobj());
        GdkWindow* gdk_window = gtk_widget_get_window(m_widget);
        gulong xid = gdk_x11_window_get_xid(gdk_window);

        system::xdraw_window_image2(xid);

        g_print("-->%lu\n", xid);
        bool has_alpha = false;
        // system::xget_render_picture_attributes(xid);
        //         system::xdraw_window_image(m_widget, xid, 200, 200);
        //  Gdk::Rectangle r = system::xget_window_attributes(xid, has_alpha);
        ///
        // m_widget = GTK_WIDGET(this->gobj());

        //        system::XDrawOnWidget(m_widget);
    }

    TestWindow::~TestWindow()
    {
        connect_signal(false);
        g_message(MSG_FREE_OBJECT, "TestWindow");
    }

    void TestWindow::connect_signal(bool connect)
    {
        if (connect) {
            m_sigc_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &TestWindow::on_timeout_draw), DF_EXPLODES_FRAMERATE);
        } else {
            m_sigc_connection.disconnect();
        }
    }

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
    // set_keep_above();
    bool TestWindow::on_timeout_draw()

    {
        m_frame_time = g_get_real_time();
        if (m_frame_time - m_start_time <= DF_EXPLODES_TIMEMAX) {
            Gtk::Widget::queue_draw();
            return true;
        }

        hide();
        return true;
    }
    void TestWindow::show_at(int x, int y)
    {
        m_start_time = g_get_real_time();
        m_frame_time = m_start_time;

        connect_signal(true);

        show();
        move(x - (m_size / 2), y - (m_size / 2));
    }

    /*bool TestWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        // Replace destination layer (bounded)
        cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);

        //    int ypos = -m_size * (int)(m_frames * (m_frame_time - m_start_time) /
        //    DF_EXPLODES_TIMEMAX); Gdk::Cairo::set_source_pixbuf(cr, m_image, 0, ypos);
        cr->paint();

        return false;
    }*/
}  // namespace docklight
