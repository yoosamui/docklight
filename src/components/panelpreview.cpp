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

#include "panelpreview.h"

#include "utils/system.h"
#include "translations.h"
// clang-format on
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace docklight
{

    PanelPreview::PanelPreview() : Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
    {
        GdkScreen* screen;
        GdkVisual* visual;

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

        // clang-format off
        // Set event masks
        add_events( Gdk::BUTTON_PRESS_MASK |
                    Gdk::BUTTON_RELEASE_MASK |
                    Gdk::SCROLL_MASK |
                    Gdk::ENTER_NOTIFY_MASK |
                    Gdk::LEAVE_NOTIFY_MASK |
                    Gdk::POINTER_MOTION_MASK
                   );
        // clang-format on

        m_size = Config()->get_preview_area();
    }

    PanelPreview::~PanelPreview()
    {
        m_sigc_updated.disconnect();
        connect_signal(false);
        g_message(MSG_FREE_OBJECT, "PanelPreview");
    }

    void PanelPreview::connect_signal(bool connect)
    {
        if (connect) {
            m_sigc_connection = Glib::signal_timeout().connect(
                sigc::mem_fun(this, &PanelPreview::on_timeout_draw), 1000 / 2);
        } else {
            m_sigc_connection.disconnect();
        }
    }

    bool PanelPreview::on_timeout_draw()

    {
        read_images();
        Gtk::Widget::queue_draw();
        return true;
    }

    guint PanelPreview::get_width()
    {
        return m_size;
    }

    void PanelPreview::read_images()
    {
        //   m_windows.clear();
        m_current_images.clear();

        if (!system::is_mutter_window_manager()) {
            // int millis = 10;
            // int event_time = gtk_get_current_event_time();
            // GdkScreen* screen = gdk_screen_get_default();
            // int current_ws_number = gdk_x11_screen_get_current_desktop(screen);
            // auto cws = wnck_screen_get_workspace(wnck::get_default_screen(), current_ws_number);
            // int cws_number = 0;

            for (auto& it : m_dockitem->get_childmap()) {
                auto child = it.second;

                m_image = Provider()->get_window_image(child->get_xid());

                if (!m_image) {
                    pixbuf::get_window_image(child->get_xid(), m_image,
                                             Config()->get_preview_image_size());
                }

                auto window = child->get_wnckwindow();
                if (!wnck_window_is_minimized(window) && !wnck_window_is_pinned(window) &&
                    wnck::is_window_on_current_desktop(window)) {
                    auto xid = it.first;

                    if (pixbuf::get_window_image(xid, m_image,
                                                 Config()->get_preview_image_size())) {
                        Provider()->set_window_image(xid, m_image);
                    }
                }

                auto pair = std::make_pair(m_image, child);
                m_current_images.push_back(pair);
            }
        } else {
            for (auto& it : m_dockitem->get_childmap()) {
                auto xid = it.first;
                if (pixbuf::get_window_image(xid, m_image, Config()->get_preview_image_size())) {
                    auto pair = std::make_pair(m_image, it.second);
                    m_current_images.push_back(pair);
                }
            }
        }
    }

    /**
     *  Event emit from Provider on close app.
     */
    void PanelPreview::on_container_updated(window_action_t action, glong xid)
    {
        if (m_visible && action == window_action_t::CLOSE) {
            read_images();

            int size = m_dockitem->get_childmap().size();
            if (!size) {
                this->close();
                return;
            }

            update();

            // int xx = 0;
            // int yy = 0;

            // int area = Config()->get_preview_area();

            // Position()->get_preview_position(m_dockitem_index, xx, yy, size, area);

            // size = m_dockitem->get_childmap().size();

            // resize(m_size * size, m_size);
            // move(xx, yy);

            // m_x = xx;
            // m_y = yy;
            //  this->show();
            //   read_images();

            // int size = m_dockitem->get_childmap().size();
            // if (size) {
            // int width = (m_size * size);
            // if (width > m_size) {
            // resize(width, m_size);
            // move(m_x, m_y);
            //}
            //} else {
            // this->close();
            //}
            //// update();
            ////  Gtk::Widget::queue_draw();
        }
    }

    void PanelPreview::show_at(int dockitem_index, std::shared_ptr<DockItemIcon> dockitem)
    {
        m_sigc_updated = Provider()->signal_update().connect(
            sigc::mem_fun(this, &PanelPreview::on_container_updated));

        connect_signal(true);
        m_dockitem_index = dockitem_index;
        m_dockitem = dockitem;

        int scalesize = get_scale_factor();
        Config()->set_image_size(scalesize);
        m_size = Config()->get_preview_area();

        read_images();

        int xx = 0;
        int yy = 0;

        int area = Config()->get_preview_area();
        int size = area * dockitem->get_childmap().size();
        Position()->get_preview_position(m_dockitem_index, xx, yy, size, area);

        size = dockitem->get_childmap().size();

        resize(m_size * size, m_size);
        move(xx, yy);

        m_x = xx;
        m_y = yy;

        show();
        m_visible = true;
    }

    void PanelPreview::update()
    {
        //      connect_signal(false);

        int scalesize = get_scale_factor();
        Config()->set_image_size(scalesize);
        m_size = Config()->get_preview_area();

        auto size = m_current_images.size();
        if (!size) {
            this->close();
        }

        if (size) resize(m_size * size, m_size);

        int x = m_x, y = m_y;
        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int area = Config()->get_preview_area();
            int size = area * m_current_images.size();

            Position()->get_preview_position(m_dockitem_index, x, y, size, area);
            size = m_dockitem->get_childmap().size();
            if (size) {
                move(x, y);

                m_x = x;
                m_y = y;
            }
        } else {
            if (Config()->get_dock_location() == dock_location_t::right) {
                x += m_size;
                move(x, y);

                m_x = x;
                m_y = y;
            }
        }

        //    connect_signal(true);
    }

    bool PanelPreview::get_visible() const
    {
        return m_visible;
    }
    void PanelPreview::hide_now()
    {
        m_sigc_updated.disconnect();
        m_sigc_connection.disconnect();

        connect_signal(false);
        m_x = m_y = 0;
        m_current_images.clear();
        m_visible = false;

        hide();
    }

    bool PanelPreview::on_enter_notify_event(GdkEventCrossing* crossing_event)
    {
        m_block_leave = false;
        return true;
    }

    bool PanelPreview::on_leave_notify_event(GdkEventCrossing* crossing_event)
    {
        if (m_block_leave) return true;

        hide_now();
        return true;
    }

    bool PanelPreview::on_button_press_event(GdkEventButton* event)
    {
        auto child = m_current_images.at(m_dockpreview_index).second;
        if (!child) return false;

        if (!system::is_mutter_window_manager()) {
            if (event->button == 3) {
                //   auto window = child->get_wnckwindow();
                wnck::activate_window(child->get_wnckwindow());
                read_images();

                Gtk::Widget::queue_draw();

                return true;
            }
        }

        // handle close button
        Gdk::Rectangle mouse_rect(event->x, event->y, 2, 2);
        if (m_close_button_rectangle.intersects(mouse_rect)) {
            m_block_leave = true;

            int xx = 0;
            int yy = 0;
            system::get_mouse_position(xx, yy);

            if (!m_anim) m_anim = Glib::RefPtr<ExplodesWindow>(new ExplodesWindow());
            m_anim->show_at(xx, yy);

            m_current_images.erase(m_current_images.begin() + m_dockpreview_index);

            wnck::close_window(child->get_wnckwindow());

            if (m_current_images.size() == 0) {
                this->close();
                return true;
            }

            update();

            return true;
        }

        //  wnck::activate_window(child->get_wnckwindow());
        //   read_images();
        wnck::activate_window(child->get_wnckwindow());
        return true;
    }

    bool PanelPreview::on_button_release_event(GdkEventButton* event)
    {
        //
        return true;
    }

    bool PanelPreview::on_motion_notify_event(GdkEventMotion* event)
    {
        get_dockpreview_index(event->x, event->y);

        if (m_last_dockpreview_index != m_dockpreview_index) {
            m_last_dockpreview_index = m_dockpreview_index;

            Gtk::Widget::queue_draw();
        }

        return true;
    }

    guint PanelPreview::get_scale_factor()
    {
        const auto workarea = device::monitor::get_workarea();
        const int max_preview_size = Config()->get_custom_image_size();
        const int num_items = m_dockitem->get_childmap().size();
        const int item_width = max_preview_size;
        int screen_width = workarea.get_width();

        // Calculate the scaling factor
        float scaling_factor =
            static_cast<float>(screen_width) / static_cast<float>(num_items * item_width);

        int image_size = std::floor(item_width * scaling_factor);

        if (image_size > max_preview_size) {
            image_size = max_preview_size;
        };

        return image_size - Config()->get_preview_area_margin();
    }

    inline guint PanelPreview::get_dockpreview_index(int mx, int my)
    {
        gint pos_x = 0;

        auto separator_size = 0;
        // auto area = Config()->get_preview_area() + separator_size;
        auto area = Config()->get_preview_area() + separator_size;
        auto size = m_dockitem->get_childmap().size();
        auto start_pos = 0;

        for (size_t idx = 0; idx < size; idx++) {
            m_dockpreview_index = -1;
            if (mx >= pos_x && mx <= pos_x + area) {
                m_dockpreview_index = idx;
                break;
            }
            pos_x += start_pos + area;
        }

        return m_dockpreview_index;
    }

    bool PanelPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        cr->set_source_rgba(0.266, 0.309, 0.361, 1.0);
        cr->paint();

        int startX = 0;
        int startY = 0;
        int margin = Config()->get_preview_area_margin();
        int idx = 0;
        int image_size = Config()->get_preview_image_size();

        for (auto& it : m_current_images) {
            Glib::RefPtr<Gdk::Pixbuf> image = it.first;
            auto child = it.second;

            if (idx == m_dockpreview_index) {
                //                cr->set_source_rgba(0.992, 0.858, 0.003, 1.0);  // 1, 1, 1, 0.2)
                //                //GELB
                cr->set_source_rgba(1, 1, 1, 0.2);
                cr->rectangle(startX, startY, m_size, margin);
                cr->fill();

                int PREVIEW_TITLE_SIZE = 20;

                int x1 = startX + m_size - PREVIEW_TITLE_SIZE - 2;
                int x2 = PREVIEW_TITLE_SIZE - 10;
                int y1 = startY + 1;
                int y2 = PREVIEW_TITLE_SIZE - 10;

                // close
                m_close_button_rectangle =
                    Gdk::Rectangle(x1 - 6, y1 + 4, PREVIEW_TITLE_SIZE, PREVIEW_TITLE_SIZE);

                cr->rectangle(m_close_button_rectangle.get_x(), m_close_button_rectangle.get_y(),
                              m_close_button_rectangle.get_width(),
                              m_close_button_rectangle.get_height());

                cr->set_source_rgba(0.992, 0.858, 0.003, 1.0);  // 1, 1, 1, 0.2)
                cr->fill();

                cr->set_source_rgba(0.870, 0.050, 0.062, 1.0);  // ROJO
                cr->set_line_width(1.5);

                x1 -= 1;
                y1 += 10;

                cr->move_to(x1, y1);
                cr->line_to(x1, y1);
                cr->line_to(x1 + x2, y1 + y2);
                cr->stroke();

                cr->move_to(x1 + x2, y1);
                cr->line_to(x1 + x2, y1);
                cr->line_to(x1, y1 + y2);

                cr->stroke();
            }

            // int centerX = m_size / 2 - image->get_width() / 2;
            // int centerY = (m_size + margin) / 2 - image->get_height() / 2;

            // cr->rectangle(startX + centerX, startY + margin, image->get_width(), m_size);
            // Gdk::Cairo::set_source_pixbuf(cr, image, startX + centerX, startY + centerY);
            // cr->fill();
            //  cr->paint();

            //  cell
            // cr->set_source_rgba(1, 1, 1, 1);
            // cairo::rounded_rectangle(cr, startX, startY, m_size, m_size, 4.0);
            // cr->stroke();

            // border
            cr->set_source_rgba(5, 1, 0, 1);
            cr->set_line_width(0.4);
            cr->rectangle(startX + 8, startY + margin - 4, m_size - 16, 1);
            cr->stroke();

            if (m_dockitem) {
                cr->save();
                std::string label = child->get_window_name();
                std::string wstring;

                if (wnck::count_in_workspace(child->get_wnckwindow(), wstring)) {
                    label = wstring + label;
                }

                draw_text(cr, startX, startY, label, idx == m_dockpreview_index);
                cr->restore();
            }

            // image cell
            // cr->set_source_rgba(1, 1, 1, 1.0);
            // cairo::rounded_rectangle(cr, startX, startY + margin, m_size, m_size -
            // margin, 4.0); cr->stroke();

            auto scaled_image = image;
            if (image->get_width() > image_size) {
                scaled_image = image->scale_simple(image_size, image_size, Gdk::INTERP_BILINEAR);
            }

            int centerX = m_size / 2 - scaled_image->get_width() / 2;
            int centerY = (m_size + margin) / 2 - scaled_image->get_height() / 2;
            cr->rectangle(startX + centerX, startY + margin, image_size, m_size - margin);

            Gdk::Cairo::set_source_pixbuf(cr, scaled_image, startX + centerX, startY + centerY);
            cr->fill();

            startX += m_size;
            startY = 0;

            idx++;
        }

        return true;
    }

    void PanelPreview::draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y,
                                 const std::string& text, bool indicator)
    {
        int offset = indicator ? 26 : 10;
        cr->rectangle(x, y + 1, m_size - offset, Config()->get_preview_area_margin() - 1);
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

        // if (indicator) {
        // cr->set_source_rgba(0, 0, 0, 1.f);  // BLACK

        //} else {
        // cr->set_source_rgba(1, 1, 1, 1.f);  // WHITE;
        //}

        cr->set_source_rgba(1, 1, 1, 1.f);  // WHITE;
        layout->show_in_cairo_context(cr);
        cr->reset_clip();  // Reset the clipping!
    }
}  // namespace docklight
