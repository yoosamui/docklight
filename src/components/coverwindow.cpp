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

#include "coverwindow.h"

#include "utils/system.h"
#include "translations.h"
// clang-format on

namespace docklight
{

    CoverWindow::CoverWindow()  //: Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
    // CoverWindow::CoverWindow() : Gtk::Window(Gtk::WindowType::WINDOW_TOPLEVEL)
    {
        // GdkScreen* screen;
        // GdkVisual* visual;

        // gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
        // screen = gdk_screen_get_default();
        // visual = gdk_screen_get_rgba_visual(screen);

        // if (visual != NULL && gdk_screen_is_composited(screen)) {
        // gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
        //}

        // set_resizable(true);
        // set_skip_taskbar_hint(true);
        // set_skip_pager_hint(true);
        set_keep_above(true);
        // gtk_window_set_keep_above(this->gobj());
        // clang-format off
        // Set event masks
        //add_events( Gdk::BUTTON_PRESS_MASK |
                    //Gdk::BUTTON_RELEASE_MASK |
                    //Gdk::SCROLL_MASK |
                    //Gdk::ENTER_NOTIFY_MASK |
                    //Gdk::LEAVE_NOTIFY_MASK |
                    //Gdk::POINTER_MOTION_MASK
                   //);
        // clang-format on
        // m_size = Config()->get_preview_image_size();
        gint x_orig, y_orig;
        // gint width, height;

        gint x, y, width, height;
        GdkWindow* root_window = gdk_get_default_root_window();

        gdk_window_get_geometry(root_window, &x, &y, &width, &height);
        GdkPixbuf* screenshot = gdk_pixbuf_get_from_window(root_window, x, y, width, height);

        if (screenshot) {
            m_image =
                Glib::wrap(screenshot, true)->scale_simple(width, width, Gdk::INTERP_BILINEAR);
        }

        // GtkWidget* toplevel = gtk_widget_get_toplevel(VGTK_WIDGET(m_window->gobj()));
        /*GdkDisplay* gdk_display = gdk_display_get_default();
        GdkWindow* gdk_window = gdk_x11_window_foreign_new_for_display(gdk_display, xid);*/

        // GtkWidget* w =gtk_window_get_default_widget (  GtkWindow* window );
        // GtkWidget* w = gtk_window_get_default_widget(*this->gobj());
        // GdkWindow* cw = gtk_widget_get_window(this->gobj());
        // auto gggg = get_window();

        // auto xid = Gtk::Window::get_window();  // Gtk::Window::get_xid();
        //        auto xid = GDK_WINDOW_XID(this->gobj());  //  wnck::minimize(xid);
        // auto xxx = gdk_x11_window_get_xid(*this->gobj());  // window: gdk::Window)
        // XID x_window = GDK_SURFACE_XID(get_native()->get_surface()->gobj());
        //  XID x_window = GDK_SURFACE_XID(this->gobj());
        // gdk_drawable_get_size(root_window, &width, &height);
        // gdk_window_get_origin(root_window, &x_orig, &y_orig);

        // GdkPixbuf* screenshot = gdk_pixbuf_get_from_drawable(NULL, root_window, NULL, x_orig,
        // y_orig, 0, 0, width, height);
        // m_image = ;  //

        m_size = 2000;  // Config()->get_preview_area();
        // image_size();
        resize(m_size, 2000);
    }

    CoverWindow::~CoverWindow()
    {
        connect_signal(false);
        g_message(MSG_FREE_OBJECT, "CoverWindow");
    }

    void CoverWindow::connect_signal(bool connect)
    {
        if (connect) {
            m_sigc_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &CoverWindow::on_timeout_draw), 1000 / 4);
        } else {
            m_sigc_connection.disconnect();
        }
    }

    bool CoverWindow::on_timeout_draw()

    {
        //   Gtk::Widget::queue_draw();
        return true;
    }

    guint CoverWindow::get_width()
    {
        return m_size;
    }

    void CoverWindow::show_at(int x, int y, std::shared_ptr<DockItemIcon> dockitem)
    {
        //  resize(m_size, m_size);
        //  move(0, 0);
        show();

        /*gint xx, yy, width, height;

        GtkWidget* widget = GTK_WIDGET(this->gobj());
        GdkWindow* cgdkwindow = gtk_widget_get_window(widget);
        GdkPixbuf* screenshot2 = gdk_pixbuf_get_from_window(cgdkwindow, xx, yy, width, height);
        if (screenshot2) {
            exit(1);
        }*/
        m_visible = true;
    }

    bool CoverWindow::get_visible() const
    {
        return m_visible;
    }
    void CoverWindow::hide_now()
    {
        connect_signal(false);
        hide();
        m_visible = false;
    }

    // bool CoverWindow::on_enter_notify_event(GdkEventCrossing* crossing_event)
    //{
    // return true;
    //}

    // bool CoverWindow::on_leave_notify_event(GdkEventCrossing* crossing_event)
    //{
    // hide_now();
    // return true;
    //}

    // bool CoverWindow::on_button_press_event(GdkEventButton* event)
    //{
    // auto child = m_current_images.at(m_dockpreview_index).second;
    // if (child) {
    // wnck::activate_window(child->get_wnckwindow());
    ////  hide_now();
    //}

    // return true;
    //}

    // bool CoverWindow::on_motion_notify_event(GdkEventMotion* event)
    //{
    // get_dockpreview_index(event->x, event->y);
    // Gtk::Widget::queue_draw();

    // return true;
    //}

    inline guint CoverWindow::get_dockpreview_index(int mx, int my)
    {
        gint pos_x = 0;
        gint pos_y = 0;

        auto separator_size = 0;
        auto area = Config()->get_preview_area() + separator_size;
        auto size = m_dockitem->get_childmap().size();
        auto maxsize = size * area;
        auto start_pos = 0;

        // get_start_pos(maxsize, pos_x, pos_y);

        for (size_t idx = 0; idx < size; idx++) {
            m_dockpreview_index = -1;
            //            if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            if (mx >= pos_x && mx <= pos_x + area) {
                m_dockpreview_index = idx;
                break;
            }
            pos_x += start_pos + area;

            //} else {  // Vertical
            // if (my >= pos_y && my <= pos_y + area) {
            // m_dockpreview_index = idx;
            // break;
            //}
            // pos_y += start_pos + area;
            //}
        }

        return m_dockpreview_index;
    }

    bool CoverWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        // cr->set_source_rgba(1, 1, 1, 1.0);
        /// cr->paint();

        Gdk::Cairo::set_source_pixbuf(cr, m_image, 0, 0);
        cr->paint();

        return true;
    }

    void CoverWindow::draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y,
                                const std::string& text)
    {
        cr->rectangle(x, y + 1, m_size - 26, Config()->get_preview_area_margin() - 1);
        cr->set_source_rgba(1, 1, 1, 0.f);  // for debuging set alpha to 1.f
        cr->clip_preserve();
        cr->stroke();

        /*//    cr->set_source_rgba(1, 1, 1, 1.f);  // for debuging set alpha to 1.f
        if (m_theme.PreviewTitleText().Stroke().Color::alpha != 0.0) {
            cr->set_source_rgba(m_theme.PreviewTitleText().Stroke().Color::red,
                                m_theme.PreviewTitleText().Stroke().Color::green,
                                m_theme.PreviewTitleText().Stroke().Color::blue,
                                m_theme.PreviewTitleText().Stroke().Color::alpha);

            cr->stroke();
        }
        */

        // http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
        Pango::FontDescription font;

        font.set_family("Monospace");
        font.set_weight(Pango::WEIGHT_NORMAL);
        font.set_size(9 * PANGO_SCALE);

        // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
        auto layout = create_pango_layout(text);

        layout->set_font_description(font);

        int text_width;
        int text_height;

        // get the text dimensions (it updates the variables -- by reference)
        layout->get_pixel_size(text_width, text_height);
        cr->move_to(x + 8, (Config()->get_preview_area_margin() / 2) - 8);

        cr->set_source_rgba(1, 1, 1, 1.f);
        layout->show_in_cairo_context(cr);
        cr->reset_clip();  // Reset the clipping!
    }
}  // namespace docklight
