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

namespace docklight
{

    DockRender::DockRender()
    {
        g_message("DockRender instantiated");
    }

    DockRender::~DockRender()
    {
        g_message("DockRender destructed.");
    }

    void DockRender::create_surface_background()
    {
        Gdk::Rectangle bckrect = position::get_background_region();
        m_background = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, bckrect.get_width(),
                                                   bckrect.get_height());

        m_bck_ctx = Cairo::Context::create(m_background);
    }
    void DockRender::draw_surface_background()
    {
        // recreate necessery.
        create_surface_background();

        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
        ctx->set_source_rgba(0.521, 0.6, 0, 1.0);
        ctx->paint();
    }

    void DockRender::create_surface_cell()
    {
        int size = config::get_dock_area();
        m_cell = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        m_cell_ctx = Cairo::Context::create(m_cell);
    }

    void DockRender::draw_surface_cell()
    {
        if (!m_cell) {
            create_surface_cell();
        }

        // clear
        m_cell_ctx->save();
        m_cell_ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);
        m_cell_ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        m_cell_ctx->paint_with_alpha(1.0);
        // m_cell_ctx->paint();

#define STROKE_SURFACE_RECT 1
#ifdef STROKE_SURFACE_RECT
        // Surface rect TEST
        m_cell_ctx->set_line_width(2.0);
        m_cell_ctx->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        m_cell_ctx->rectangle(0, 0, m_cell->get_width(), m_cell->get_height());
        m_cell_ctx->stroke();
#endif

        m_cell_ctx->restore();
    }

    void DockRender::create_surface_icon()
    {
        int size = config::get_icon_size();

        m_icon = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        m_icon_ctx = Cairo::Context::create(m_icon);
    }

    void DockRender::draw_surface_icon(std::shared_ptr<DockItemIcon>& item)
    {
        g_assert(m_cell);

        int size = config::get_icon_size();
        g_assert(m_background);

        if (!m_icon) {
            create_surface_icon();
        }

        // clear
        m_icon_ctx->save();
        m_icon_ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);

        auto icon = item->get_icon(size);

        // rotation test
        // m_icon_ctx->translate(0, 64);
        // rotation = DegreesToRadians(90);
        // m_icon_ctx->rotate(rotation);
        // paint surface rec

        m_icon_ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        m_icon_ctx->set_line_width(2.0);
        m_icon_ctx->set_source_rgb(0.0, 0.0, 0.0);
        m_icon_ctx->rectangle(0, 0, m_icon->get_width(), m_icon->get_height());
        m_icon_ctx->fill();

        Gdk::Cairo::set_source_pixbuf(m_icon_ctx, icon, 0, 0);
        m_icon_ctx->paint();

        m_icon_ctx->restore();

        guint centerX = (config::get_dock_area() / 2) - (m_icon->get_width() / 2);
        m_cell_ctx->set_source(m_icon, centerX, config::get_dock_area_margin());
        m_cell_ctx->paint();
    }

    void DockRender::create_surface_indicator(std::shared_ptr<DockItemIcon>& item)
    {
        // TODO 4  put it in config
        int height = 4;
        int width = config::get_icon_size();

        m_indicator = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
        m_indicator_ctx = Cairo::Context::create(m_indicator);
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
        m_indicator_ctx->set_source_rgba(0, 0.243, 0.541, 1.0);

        if (item->get_childmap().size() == 1) {
            m_indicator_ctx->rectangle(0, 0, m_indicator->get_width(),
                                       m_indicator->get_height() - 1);

        } else {
            m_indicator_ctx->rectangle(0, 0, m_indicator->get_width() / 2 - 4,
                                       m_indicator->get_height() - 1);
            m_indicator_ctx->rectangle(m_indicator->get_width() / 2 + 4, 0,
                                       m_indicator->get_width() - 8, m_indicator->get_height() - 1);
        }
        m_indicator_ctx->fill();
        m_indicator_ctx->restore();

        // add to cell surface
        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            int centerX = (config::get_dock_area() / 2) - (m_indicator->get_width() / 2);
            m_cell_ctx->set_source(m_indicator, centerX,
                                   m_cell->get_height() - m_indicator->get_height());

        } else {
            int centerX = (config::get_dock_area() / 2) - (m_indicator->get_width() / 2);
            m_cell_ctx->set_source(m_indicator, centerX,
                                   m_cell->get_height() - m_indicator->get_height());
        }

        m_cell_ctx->paint();
    }

    bool DockRender::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        auto container = get_dockcontainer();
        auto appmap = container->get_appmap();

        m_posX = 0;
        m_posY = 0;

        draw_surface_background();

        guint tag = 0;
        for (auto it = appmap.begin(); it != appmap.end(); it++) {
            // auto dockitem = it->second;
            std::shared_ptr<DockItemIcon> dockitem;
            dockitem_any_cast<std::shared_ptr<DockItemIcon>>(it->second, dockitem);

            dockitem->set_tag(tag++);

            draw_surface_cell();
            draw_surface_icon(dockitem);
            draw_surface_indicator(dockitem);

            m_bck_ctx->set_source(m_cell, m_posX, m_posY);
            m_bck_ctx->paint();

            guint separator_size = config::get_separator_size();
            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                m_posX += config::get_dock_area() + separator_size;
            } else {
                m_posY += config::get_dock_area() + separator_size;
            }
        }

        cr->set_source(m_background, 0, 0);
        cr->paint();

        return false;
    }
}  // namespace docklight
