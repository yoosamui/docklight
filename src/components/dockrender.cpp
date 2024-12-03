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
#include "dockrender.h"
// clang-format on

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

namespace docklight
{

    DockRender::DockRender()
    {
        m_position = Position();

        g_message("Create DockRender.");
    }

    DockRender::~DockRender()
    {
        g_message("DockRender destructed.");
    }

    void DockRender::create_surface_background()
    {
        Gdk::Rectangle bckrect = m_position->get_window_geometry();
        m_background = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, bckrect.get_width(),
                                                   bckrect.get_height());

        m_bck_ctx = Cairo::Context::create(m_background);
    }
    void DockRender::draw_surface_background()
    {
        // recreate this surface is mandatory.
        create_surface_background();
        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
        // ctx->set_source_rgba(0.266, 0.309, 0.361, 1.0);
        // ctx->paint();

//#define STROKE_BCK_RECT 1
#ifdef STROKE_BCK_RECT
        ctx->set_line_width(1.0);
        ctx->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        ctx->rectangle(0, 0, m_background->get_width(), m_background->get_height());
        ctx->stroke();
#endif

        ctx->set_source_rgba(0.266, 0.309, 0.361, 1.0);
        cairo::rounded_rectangle(ctx, 0, 0, m_background->get_width(), m_background->get_height(),
                                 4.0);
        ctx->fill();
    }

    void DockRender::create_surface_cell()
    {
        int size = Config()->get_dock_area();
        m_cell = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        m_cell_ctx = Cairo::Context::create(m_cell);
    }

    void DockRender::draw_surface_cell(std::shared_ptr<DockItemIcon>& item)
    {
        //        if (!m_cell) {
        create_surface_cell();
        //        }

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

    void DockRender::create_surface_icon()
    {
        int size = Config()->get_icon_size();

        m_icon = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        m_icon_ctx = Cairo::Context::create(m_icon);
    }

    void DockRender::draw_surface_icon(std::shared_ptr<DockItemIcon>& item)
    {
        g_assert(m_cell);
        g_assert(m_background);

        // if (!m_icon) {
        create_surface_icon();
        // }

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

    void DockRender::create_surface_indicator(std::shared_ptr<DockItemIcon>& item)
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

    inline void DockRender::get_start_pos(const gint maxsize, gint& x, gint& y)
    {
        auto center = 0;
        x = y = 0;

        if (Config()->get_dock_alignment() != dock_alignment_t::fill) return;

        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::center) {
                center = m_position->get_workarea().get_width() / 2 - maxsize / 2;
                x = m_position->get_workarea().get_x() + center;

            } else if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::end) {
                x = m_position->get_workarea().get_width() - maxsize;
            }

        } else {  // Vertical
            if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::center) {
                auto vertical_addition = Config()->get_dock_area() + Config()->get_separator_size();
                center =
                    m_position->get_workarea().get_height() / 2 - (maxsize + vertical_addition) / 2;
                y = m_position->get_workarea().get_y() + center;

            } else if (Config()->get_dock_icon_alignment() == dock_icon_alignment_t::end) {
                y = m_position->get_workarea().get_height() - maxsize;
            }
        }
    }

    void DockRender::draw_surface_indicator(std::shared_ptr<DockItemIcon>& item)
    {
        g_assert(m_cell);

        if (!m_indicator) {
            create_surface_indicator(item);
        }

        // clear
        m_indicator_ctx->save();
        m_indicator_ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);
        m_indicator_ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        m_indicator_ctx->paint_with_alpha(1.0);

        // draw indicators
        m_indicator_ctx->set_line_width(2.0);
        // Yellow
        m_indicator_ctx->set_source_rgba(0.980, 0.929, 0.50, 1.0);

        if (item->get_childmap().size() > 0) {
            if (item->get_childmap().size() == 1) {
                m_indicator_ctx->rectangle(4, 0, (m_indicator->get_width()) - 8,
                                           m_indicator->get_height() - 1);

            } else {
                m_indicator_ctx->rectangle(2, 0, (m_indicator->get_width() / 2) - 4,
                                           m_indicator->get_height() - 1);

                m_indicator_ctx->rectangle((m_indicator->get_width() / 2) + 2, 0,
                                           (m_indicator->get_width() / 2) - 4,
                                           m_indicator->get_height() - 1);
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

    bool DockRender::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        m_posX = 0;
        m_posY = 0;

        guint separator_size = Config()->get_separator_size();
        auto area = Config()->get_dock_area() + separator_size;
        auto data = Provider()->data();
        auto maxsize = data.size() * area;

        get_start_pos(maxsize, m_posX, m_posY);

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
            //  }
        }

        cr->set_source(m_background, 0, 0);
        cr->paint();
        return true;
    }

}  // namespace docklight
