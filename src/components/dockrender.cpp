//  clang-format off
#include "dockrender.h"
//  clang-format on

namespace docklight
{

    DockRender::DockRender()
    {
        g_message("DockRender instantiated");
        //
    }

    DockRender::~DockRender() {}

    void DockRender::create_surface_background()
    {
        Gdk::Rectangle bckrect = position::get_background_region();
        m_background = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, bckrect.get_width(),
                                                   bckrect.get_height());

        m_bck_ctx = Cairo::Context::create(m_background);
    }
    void DockRender::draw_background()
    {
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

        m_surface_icon = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        m_icon_ctx = Cairo::Context::create(m_surface_icon);
    }

    void DockRender::draw_surface_icon(const Glib::RefPtr<DockItem>& item)
    {
        int size = config::get_icon_size();
        g_assert(m_background);

        if (!m_surface_icon) {
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
        m_icon_ctx->rectangle(0, 0, m_surface_icon->get_width(), m_surface_icon->get_height());
        m_icon_ctx->fill();

        Gdk::Cairo::set_source_pixbuf(m_icon_ctx, icon, 0, 0);
        m_icon_ctx->paint();

        m_icon_ctx->restore();

        int centerX = (config::get_dock_area() / 2) - (m_surface_icon->get_width() / 2);
        m_cell_ctx->set_source(m_surface_icon, centerX, config::get_dock_area_margin());
        m_cell_ctx->paint();
    }

    bool DockRender::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        auto container = get_dockcontainer();
        auto appmap = container->get_appmap();

        m_posX = 0;
        m_posY = 0;

        draw_background();

        // m_bck_ctx = Cairo::Context::create(m_background);

        guint tag = 0;
        for (auto it = appmap.begin(); it != appmap.end(); it++) {
            auto dockitem = it->second;
            dockitem->set_tag(tag++);

            draw_surface_cell();
            draw_surface_icon(dockitem);
            //          draw_indicator(dockitem);

            // bck_ctx = Cairo::Context::create(m_background);
            m_bck_ctx->set_source(m_cell, m_posX, m_posY);
            m_bck_ctx->paint();
            // add surfaceicon to the cell surface

            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                m_posX += config::get_dock_area() + config::get_separator_margin();
            } else {
                // m_posY += config::get_icon_size() + config::get_separator_margin() +
                // config::get_separator_margin();

                m_posY += config::get_dock_area() + config::get_separator_margin();
                //   config::get_separator_margin();
            }
        }

        cr->set_source(m_background, 0, 0);
        cr->paint();

        return false;
    }
}  // namespace docklight
