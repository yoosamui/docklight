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
#include "panelrender.h"
// clang-format on

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace docklight
{

    PanelRender::PanelRender()
    {
        m_position = Position();
        Autohide()->signal_hide().connect(sigc::mem_fun(this, &PanelRender::on_autohide_update));

        //  m_theme->= Config()->get_theme();
        g_message("Create PanelRender.");
    }

    PanelRender::~PanelRender()
    {
        g_message("PanelRender destructed.");
    }

    void PanelRender::create_surface_background()
    {
        Gdk::Rectangle bckrect = m_position->get_window_geometry();
        m_background = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, bckrect.get_width(),
                                                   bckrect.get_height());

        m_bck_ctx = Cairo::Context::create(m_background);
    }
    void PanelRender::draw_surface_background()
    {
        // recreate this surface is mandatory.
        create_surface_background();
        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
        ctx->set_source_rgba(0, 0, 0, 0.0);
        ctx->paint();

//#define STROKE_BCK_RECT 1
#ifdef STROKE_BCK_RECT
        ctx->set_line_width(1.0);
        ctx->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        ctx->rectangle(0, 0, m_background->get_width(), m_background->get_height());
        ctx->stroke();
#endif

        Gdk::Rectangle rect =
            Gdk::Rectangle(0, 0, m_background->get_width(), m_background->get_height());
        cairo::fill(ctx, m_theme->Panel(), m_theme->PanelGradient(), rect);
        cairo::stroke(ctx, m_theme->Panel(), rect);
    }

    void PanelRender::create_surface_cell()
    {
        int size = Config()->get_dock_area();
        m_cell = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        m_cell_ctx = Cairo::Context::create(m_cell);
    }

    void PanelRender::draw_surface_cell(std::shared_ptr<DockItemIcon>& item)
    {
        create_surface_cell();

        // clear
        m_cell_ctx->save();
        m_cell_ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);
        m_cell_ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        m_cell_ctx->paint_with_alpha(1.0);

//#define STROKE_SURFACE_RECT 1
#ifdef STROKE_SURFACE_RECT
        // Surface rect TEST
        m_cell_ctx->set_line_width(1.0);
        m_cell_ctx->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        m_cell_ctx->rectangle(0, 0, m_cell->get_width(), m_cell->get_height());
        m_cell_ctx->stroke();
#endif

        if (m_mouse_enter && item->get_tag() == m_dockitem_index) {
            m_cell_ctx->set_source_rgba(1.0, 1.0, 1.0, 0.1);
            m_cell_ctx->paint();
        }

        if (!m_mouse_enter && item->get_tag() && item->get_tag() == m_dockitem_active_index) {
            m_cell_ctx->set_source_rgba(0.0, 1.0, 1.0, 0.3);
            m_cell_ctx->paint();

            // m_cell_ctx->set_operator(Cairo::Operator::OPERATOR_ADD);
            // m_cell_ctx->paint_with_alpha(0.6);
            // m_cell_ctx->set_operator(Cairo::Operator::OPERATOR_OVER);
        }

        m_cell_ctx->restore();
    }

    void PanelRender::create_surface_icon()
    {
        int size = Config()->get_icon_size();

        m_icon = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        m_icon_ctx = Cairo::Context::create(m_icon);
    }

    void PanelRender::draw_surface_icon(std::shared_ptr<DockItemIcon>& item)
    {
        g_assert(m_cell);
        g_assert(m_background);

        create_surface_icon();

        // clear
        m_icon_ctx->save();
        m_icon_ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);

        int size = Config()->get_icon_size();

        if (m_mouse_enter && item->get_tag() == m_dockitem_index) {
            size -= 2;
        }

        auto icon = item->get_icon(size);

        m_icon_ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        m_icon_ctx->set_line_width(2.0);
        m_icon_ctx->set_source_rgb(0.0, 0.0, 0.0);
        m_icon_ctx->rectangle(0, 0, m_icon->get_width(), m_icon->get_height());
        m_icon_ctx->fill();

        Gdk::Cairo::set_source_pixbuf(m_icon_ctx, icon, 0, 0);
        m_icon_ctx->paint();

        m_icon_ctx->restore();

        guint centerX = (Config()->get_dock_area() / 2) - (m_icon->get_width() / 2);
        m_cell_ctx->set_source(m_icon, centerX, Config()->get_dock_area_margin() / 2);
        m_cell_ctx->paint();
    }

    void PanelRender::create_surface_indicator(std::shared_ptr<DockItemIcon>& item)
    {
        int height = Config()->DEF_INDICATOR_SIZE;
        int width = Config()->get_icon_size();

        float factor = std::abs(Config()->get_icon_size() / Config()->DEF_ICON_MAXSIZE);
        height += factor;

        if (height < Config()->DEF_INDICATOR_SIZE) {
            height = Config()->DEF_INDICATOR_SIZE;
        }

        m_indicator = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
        m_indicator_ctx = Cairo::Context::create(m_indicator);
    }

    void PanelRender::draw_surface_indicator(std::shared_ptr<DockItemIcon>& item)
    {
        g_assert(m_cell);
        create_surface_indicator(item);

        // clear
        m_indicator_ctx->save();
        m_indicator_ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);
        m_indicator_ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        m_indicator_ctx->paint_with_alpha(1.0);

        // TODO style here
        //  draw indicators
        m_indicator_ctx->set_line_width(0.5);
        // Yellow
        //       m_indicator_ctx->set_source_rgba(0.980, 0.929, 0.50, 1.0);

        //  auto m_theme->= Config()->get_theme();
        m_indicator_ctx->set_source_rgba(m_theme->PanelIndicator().Stroke().Color::red,
                                         m_theme->PanelIndicator().Stroke().Color::green,
                                         m_theme->PanelIndicator().Stroke().Color::blue,
                                         m_theme->PanelIndicator().Stroke().Color::alpha);

        auto const indicator_type = Config()->get_indicator_type();

        if (indicator_type == dock_indicator_type_t::dots) {
            int center = (m_cell->get_width() / 2);

            if (item->get_childmap().size() == 1) {
                m_indicator_ctx->arc(center - 7, 5, 1.6, 0, 2 * M_PI);
            } else if (item->get_childmap().size() > 1) {
                m_indicator_ctx->arc(center - 7 - 4, 5, 1.6, 0, 2 * M_PI);
                m_indicator_ctx->arc(center - 7 + 4, 5, 1.6, 0, 2 * M_PI);
            }
        } else {
            int hsize = m_indicator->get_height() / 4;
            int icon_size = Config()->get_icon_size();
            int y = m_indicator->get_height() - hsize;

            if (icon_size > 0 && icon_size <= 16) hsize = m_indicator->get_height() / 9;
            if (icon_size >= 16 && icon_size <= 32) hsize = m_indicator->get_height() / 8;
            if (icon_size >= 32 && icon_size <= 64) hsize = m_indicator->get_height() / 7;
            if (icon_size >= 64 && icon_size <= 128) hsize = m_indicator->get_height() / 4;

            if (item->get_childmap().size() == 1) {
                m_indicator_ctx->rectangle(4, y, (m_indicator->get_width()) - 8, hsize);

            } else if (item->get_childmap().size() > 1) {
                m_indicator_ctx->rectangle(2, y, (m_indicator->get_width() / 2) - 4, hsize);

                m_indicator_ctx->rectangle((m_indicator->get_width() / 2) + 2, y,
                                           (m_indicator->get_width() / 2) - 4, hsize);
            }
        }

        m_indicator_ctx->fill();
        m_indicator_ctx->restore();

        // add to cell surface
        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int centerX = (Config()->get_dock_area() / 2) - (m_indicator->get_width() / 2);
            m_cell_ctx->set_source(m_indicator, centerX,
                                   m_cell->get_height() - m_indicator->get_height());

        } else {
            int centerX = (Config()->get_dock_area() / 2) - (m_indicator->get_width() / 2);
            m_cell_ctx->set_source(m_indicator, centerX,
                                   m_cell->get_height() - m_indicator->get_height());
        }

        m_cell_ctx->paint();
    }

    void PanelRender::on_autohide_update(int x, int y)
    {
        m_offsetX = x;
        m_offsetY = y;

        Gtk::Widget::queue_draw();
    }

    bool PanelRender::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        int m_posX = 0;
        int m_posY = 0;

        guint separator_size = Config()->get_separator_size();
        auto area = Config()->get_dock_area() + separator_size;
        auto data = Provider()->data();
        //  auto maxsize = data.size() * area;

        Position()->get_start_pos(m_posX, m_posY);
        // get_start_pos(maxsize, m_posX, m_posY);

        // g_print("%d x %d\n", m_posX, m_posY);

        draw_surface_background();
        guint tag = 0;
        for (auto& dockitem : data) {
            draw_surface_cell(dockitem);
            draw_surface_icon(dockitem);
            draw_surface_indicator(dockitem);

            if (dockitem->get_visible()) {
                m_bck_ctx->set_source(m_cell, m_posX, m_posY);
                m_bck_ctx->paint();
            }

            if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                m_posX += area;
            } else {
                m_posY += area;
            }

            dockitem->set_tag(tag++);
        }

        cr->set_source(m_background, m_offsetX, m_offsetY);
        cr->paint();

        return true;
    }

}  // namespace docklight
