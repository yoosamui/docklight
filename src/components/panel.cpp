// clang-format off
#include <glibmm/i18n.h>

#include "components/panel.h"
#include "translations.h"

#include <iostream>

#include <gdkmm/general.h>  // set_source_pixbuf()
//#include <glibmm/timer.h>
//#include <gtkmm/drawingarea.h>
// clang-format on

namespace docklight
{

    Panel::Panel()
    {
        //    m_appupdater.signal_update().connect(sigc::mem_fun(this, &Panel::on_update));
        //    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Panel::on_timeout), 1000 / 12);
        // size(1o00, 100);
        Glib::signal_timeout().connect(sigc::mem_fun(this, &Panel::on_timeout_draw), 1000 / 30);

        //
        //
        // m_surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, 64, 64);
        m_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 64, 64);
        // ..m_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, 64, 64);
        // Cairo::RefPtr<Cairo::Surface> surface =
        // Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    }

    Panel::~Panel()
    {
        // std::cout << MSG_FREE_OBJECT << "\n" << std::endl;
        g_print(MSG_FREE_OBJECT, "Panel");
        g_print("\n");
    }

    bool Panel::on_timeout_draw()
    {
        g_message("TIMELLLLL");
        DrawIcon();
        //    Gtk::Widget::queue_draw();
        return false;
    }

    /*const Cairo::RefPtr<Cairo::ImageSurface>& Panel::DrawIcon()
    {
        int size = 64;
        if (!m_surfaceIcon) {
            //
            m_surfaceIcon = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        }
        std::string iconname("data/images/docklight.home.ico");
        auto home_pixbuf = Gdk::Pixbuf::create_from_file(iconname, size, size);
        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_surfaceIcon);

        Gdk::Cairo::set_source_pixbuf(ctx, home_pixbuf, 0, 0);
        ctx->paint();

        double draw_value_darken = 0.8;
        ctx->rectangle(0, 0, m_surfaceIcon->get_width(), m_surfaceIcon->get_height());
        ctx->set_source_rgba(0, 0, 0, draw_value_darken);
        ctx->set_operator(Cairo::Operator::OPERATOR_ATOP);
        // icon_cr.set_source_rgba(0, 0, 0, draw_value.darken);
        //   icon_cr.set_operator(Cairo.Operator.ATOP);
        ctx->fill();
        ctx->set_operator(Cairo::Operator::OPERATOR_OVER);

        return m_surfaceIcon;
    }*/
    const Cairo::RefPtr<Cairo::ImageSurface>& Panel::DrawIcon()
    {
        int size = 64;
        if (!m_surfaceIcon) {
            //
            m_surfaceIcon = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        }
        std::string iconname("data/images/docklight.home.ico");
        auto home_pixbuf = Gdk::Pixbuf::create_from_file(iconname, size, size);
        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_surfaceIcon);

        Gdk::Cairo::set_source_pixbuf(ctx, home_pixbuf, 0, 0);
        ctx->paint();

        double draw_value_darken = 0.8;
        ctx->rectangle(0, 0, m_surfaceIcon->get_width(), m_surfaceIcon->get_height());
        ctx->set_source_rgba(0, 0, 0, draw_value_darken);
        ctx->set_operator(Cairo::Operator::OPERATOR_ATOP);
        // icon_cr.set_source_rgba(0, 0, 0, draw_value.darken);
        //   icon_cr.set_operator(Cairo.Operator.ATOP);
        ctx->fill();
        ctx->set_operator(Cairo::Operator::OPERATOR_OVER);

        return m_surfaceIcon;
    }
    bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        g_message("PANELLLLLLLLLLLLLLLLLLLLLLLLL");
        cr->save();  // save the state of the context
        cr->set_source_rgba(0.521, 0.6, 0, 1.0);
        // cr->set_source_rgb(0.86, 0.85, 0.47);
        cr->paint();    // fill image with the color
        cr->restore();  // color is back to black now
        if (!m_surfaceIcon) {
            return false;
        }
        cr->set_source(m_surfaceIcon, 0, 0);
        cr->paint();

        return false;
        // cairo_surface_t* m_target = nullptr;
        // cairo_surface_t* result =
        // cairo_surface_create_for_rectangle(m_target, 10.0, 10.0, 100.0, 100.0);
        //        Cairo::Surface::Surface(cairo_surface_t * cobject, bool has_reference = false)
        //        Cairo::Surface::Surface(m_target);
        //  Cairo::RefPtr<Cairo::ImageSurface> imSur =
        // Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, 64, 64);
        //    Cairo::RefPtr<Cairo::Surface>
        //        ddd = cairo_surface_create_for_rectangle(m_target, 10.0, 10.0, 100.0, 100.0);
        // Setting the compositing operator
        // Replace destination layer (bounded)
        //   cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        //
        //
        //

        /*
        cr->rectangle(0, 0, 64, 64);
        cr->fill();

        cr->move_to(0, 0);
        cr->set_source_rgb(0, 1, 0);
        cr->line_to(0, 10);
        c r->stroke();*/

        // Create Cairo context for the image surface.
        /*Cairo::RefPtr<Cairo::Context> refContext = Cairo::Context::create(m_surface);

        // Create a radial gradient (pattern)
        int x_off = 1;
        int y_off = 1;
        int cent_point_radius = 4;

        Cairo::RefPtr<Cairo::RadialGradient> refPattern =
            Cairo::RadialGradient::create(x_off, y_off, 0, x_off, y_off, cent_point_radius);*/

        // Set the pattern as the source for the context.
        //  refContext->set_source(refPattern);
        //  refContext->fill();

        // Add a closed path and fill...

        //        cr->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        //        cr->set_source_rgba(0, 0.282, 0.62, 1.0);
        // cr->set_source(m_surface, 100, 0);
        // cr->set_source_rgb(1, 1, 1);
        //  cr->set_source_rgba(0, 0.282, 0.62, 1.0);
        // cr->paint();
        // https://www.cairographics.org/documentation/cairomm/reference/examples.html
        //
        //////////////////////////////////////////////////////////////////////////////////
        /*cr->set_source(m_surface, 0, 0);

        // cr->set_source_pixbuf(home_pixbuf, 0, 0);

        cr->save();  // save the state of the context
        cr->set_source_rgba(0.521, 0.6, 0, 1.0);
        // cr->set_source_rgb(0.86, 0.85, 0.47);
        cr->paint();    // fill image with the color
        cr->restore();  // color is back to black now

        cr->save();
        // draw a border around the image
        cr->set_line_width(20.0);  // make the line wider
        cr->rectangle(0.0, 0.0, m_surface->get_width(), m_surface->get_height());
        cr->stroke();

        cr->set_source_rgba(0.050, 0.372, 0.71, 0.9);
        // draw a circle in the center of the image
        cr->arc(m_surface->get_width() / 2.0, m_surface->get_height() / 2.0,
                m_surface->get_height() / 4.0, 0.0, 2.0 * M_PI);
        cr->stroke();

        cr->restore();  // color is back to black now
        // draw a diagonal line
        cr->set_source_rgba(0, 0, 0, 0.9);

        cr->save();
        cr->move_to(m_surface->get_width() / 4.0, m_surface->get_height() / 4.0);
        cr->line_to(m_surface->get_width() * 3.0 / 4.0, m_surface->get_height() * 3.0 / 4.0);
        cr->stroke();
        cr->restore();

        // cr->set_source(m_surface, 0, 0);
        // cr->save();
        std::string iconname("data/images/docklight.home.ico");
        auto home_pixbuf = Gdk::Pixbuf::create_from_file(iconname, 32, 32);
        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_surface);

        Gdk::Cairo::set_source_pixbuf(ctx, home_pixbuf, 40, 0);
        ctx->paint();*/
        /////////////////////////////////////////////////////////////////////////////////////////
        // x->restore();  // color is back to black now
        /*#ifdef CAIRO_HAS_PNG_FUNCTIONS

                std::string filename = "image.png";
                m_surface->write_to_png(filename);

                std::cout << "Wrote png file \"" << filename << "\"" << std::endl;

        #else

                std::cout << "You must compile cairo with PNG support for this example to work."
                          << std::endl;

        #endif*/
        return false;
    }
}  // namespace docklight

