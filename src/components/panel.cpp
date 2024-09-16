// clang-format off
#include <glibmm/i18n.h>
#include "components/panel.h"
#include "translations.h"
#include "components/position.h"
#include <iostream>
#include <gdkmm/general.h>  // set_source_pixbuf()
//#include <glibmm/timer.h>
//#include <gtkmm/drawingarea.h>
// clang-format on
#include <cmath>
namespace docklight
{

    Panel::Panel()
    {
        // MyClass* d = MyClass::getInstance();
        // d->doSomething();
        //       m_appupdater.signal_update().connect(sigc::mem_fun(this, &Panel::on_update));
        //       Glib::signal_timeout().connect(sigc::mem_fun(*this, &Panel::on_timeout), 1000 /
        //       12);
        //    size(1o00, 100);

        //
        //
        // m_surface = Cairo::ImageSurface::create(Cairo::FORMAT_RGB24, 64, 64);
        m_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, 64, 64);
        // ..m_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::ARGB32, 64, 64);
        // Cairo::RefPtr<Cairo::Surface> surface =
        // Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
        //  draw_icon();
        // Set event masks
        add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK |
                   Gdk::SMOOTH_SCROLL_MASK | Gdk::POINTER_MOTION_HINT_MASK |
                   Gdk::FOCUS_CHANGE_MASK | Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK |
                   Gdk::POINTER_MOTION_HINT_MASK | Gdk::POINTER_MOTION_MASK);
    }

    void Panel::init()
    {
        // frame_time = GLib.get_monotonic_time();
        // m_app_provider = Glib::RefPtr<AppProvider>(new AppProvider());

        // DockItem* m_dockitem = DockItem::getInstance();

        //..const auto dockitem = DockItemProvider::create();
        //  dockitem->get_childrens();
    }

    Panel::~Panel()
    {
        // std::cout << MSG_FREE_OBJECT << "\n" << std::endl;
        g_print(MSG_FREE_OBJECT, "Panel");
        g_print("\n");
    }
    bool Panel::on_enter_notify_event(GdkEventCrossing* crossing_event)
    {
        m_sigc_draw =
            Glib::signal_timeout().connect(sigc::mem_fun(this, &Panel::on_timeout_draw), 1000 / 15);
        m_mouse_enter = true;
        //   g_print("On Enter\n");
        // m_mouse_in = true;
        return true;
    }

    bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
    {
        m_sigc_draw.disconnect();
        m_mouse_enter = false;
        // g_print("On leave\n");
        // m_mouse_in = false;
        return true;
    }
    static bool anim = false;
    bool Panel::on_timeout_draw()
    {
        //   g_message("TIMELLLLL");

        Gtk::Widget::queue_draw();

        /*auto max_click_time = LaunchBounceTime;
        if (anim) {
            m_frame_time = g_get_real_time();
            m_LastClicked = g_get_real_time();
            // check for and calculate click-animation
            LaunchBounceTime = 600;
            max_click_time *= 1000;
            anim = true;
        }

        auto click_time = std::max(0.0f, m_frame_time - m_LastClicked);
        m_posY += easing_bounce(click_time, max_click_time, 2);
        g_print("%d\n", (int)m_posY);

        [>position_manager.LaunchBounceHeight <]
        //  auto urgent_duration = 600 * 1000;
        //  auto urgent_time = std::max(0.f, frame_time - item.LastUrgent);*/

        return true;

        if (m_animation_time < 1.0f) {
            m_frame_time = g_get_real_time();
            m_initTime = 0.0f;  // g_get_real_time();
            m_endTime = 10.0f;  // m_initTime + 10.0f;
            // auto endTime = m_initTime + 12;  // m_easing_duration;
            // float startPosition = 0.f;
            // float endPosition = 0.f;
            //  m_animation_time = 0;
        }

        float pos = 0.0f;
        float startPosition = 0.f;
        float endPosition = 74.f;
        // if (!nearly_equal(m_animation_time, endPosition, 2)) {
        m_posY = easing::map_clamp(m_animation_time, m_initTime, m_endTime, startPosition,
                                   endPosition, &easing::bounce::easeOut);

        if (m_posY != endPosition) {
            // std::cout << pos << ",  " << endPosition << std::endl;
            g_print("%d\n", (int)m_posY);

            m_animation_time += 0.4f;  // g_get_real_time() - m_frame_time;
        } else {
            m_posY = 0;
            m_animation_time = 0.f;
        }

        // easing_util::map_clamp(m_animation_time, m_initTime, endTime, startPosition,
        //                                   endPosition, &easing_util::linear::easeOut);
        //  GLib.get_monotonic_time();
        //    initialize_frame(frame_time);
        //    widget widget.queue_draw();

        return true;
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

    void Panel::draw_icon()
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
    }

    void Panel::draw_background()
    {
        // auto win = position::get_window();
        Gdk::Rectangle bckrect = position::get_background_region();

        if (!m_background) {
            m_background = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, bckrect.get_width(),
                                                       bckrect.get_height());
        }
        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
        ctx->rectangle(0, 0, m_background->get_width(), m_background->get_height());
        ///  ctx->set_source_rgba(0, 0, 0, draw_value_darken);
        ctx->set_source_rgba(0.521, 0.6, 0, 1.0);
        ctx->fill();
        // ctx->set_operator(Cairo::Operator::OPERATOR_ATOP);
    }

    void Panel::draw_background(const Cairo::RefPtr<Cairo::Context>& cr,
                                Gdk::Rectangle background_rect, int x_offset, int y_offset)
    {
        if (!m_background || background_rect.get_width() <= 0 ||
            background_rect.get_height() <= 0) {
            return;
        }

        if (!m_background || m_background->get_width() != background_rect.get_width() ||
            m_background->get_height() != background_rect.get_height()) {
            // create a new surface;
            g_error("CREATE A NEW SUTFACE");
            // m_background = theme.create_background(background_rect.width, background_rect.height,
            // position_manager.Position, main_buffer);
        }

        cr->set_source(m_background, background_rect.get_x() + x_offset,
                       background_rect.get_y() + y_offset);

        if (m_hide_progress > 0.0 /*&& theme.CascadeHide*/) {
            int x = 0, y = 0;
            // position_manager.get_background_padding(out x, out y);
            x_offset -= (int)(x * m_hide_progress);
            y_offset -= (int)(y * m_hide_progress);
        }

        // draw to surface
        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
        ctx->set_source_rgba(0.521, 0.6, 0, 1.0);
        ctx->paint();
    }

    void Panel::aplay_layers()
    {
        if (!m_background || !m_surfaceIcon) {
            return;
        }

        Cairo::RefPtr<Cairo::Context> bck_ctx = Cairo::Context::create(m_background);
        bck_ctx->set_source(m_surfaceIcon, 0, 0);
        bck_ctx->paint();
    }

    void Panel::draw_glow()
    {
        auto diff = std::max(0L, m_frame_time - 2L);

        //  if (diff >= theme.GlowTime * 1000) return;
        //
        g_print("%lu\n", diff);
    }

    bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        if (!m_background) {
            auto win = position::get_window();
            m_background = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, win->get_width(),
                                                       win->get_height());

            // g_message("%d x %d \n", win->get_width(), win->get_height());
        }

        auto background_rect = position::get_background_region();
        draw_background(cr, background_rect, 0, m_posY);
        // int x_offset = 0, y_offset = 0;
        // Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);

        // ctx->set_source_rgba(1, 1, 1, 1.0);
        // ctx->paint();

        // cr->set_source(m_background, 0, 0);
        // cr->fill();
        // cr->paint();

        cr->paint();
        return false;
        // controller.window.get_window ().get_scale_factor ();
        //            double window_scale_factor = 0.0;
        //            m_main_surface->set_device_scale(window_scale_factor,
        //            window_scale_factordouble);

        // main_buffer =
        // new Surface.with_cairo_surface(win_rect.width, win_rect.height, cr.get_target());
        // main_buffer.Internal.set_device_scale(window_scale_factor, window_scale_factor);

        /*///   if (!m_background) return false;
        //        if (!m_background) {
        draw_background();
        draw_icon();

        draw_glow();

        aplay_layers();
        //  }

        auto x_offset = 0, y_offset = 10;

        //   g_message("PANELLLLLLLLLLLLLLLLLLLLLLLLL");
        // cr->save();  // save the state of the context
        // cr->set_source_rgba(0.521, 0.6, 0, 1.0);
        //// cr->set_source_rgb(0.86, 0.85, 0.47);
        // cr->paint();    // fill image with the color
        // cr->restore();  // color is back to black now
        // if (!m_surfaceIcon) {
        // return false;
        //}

        cr->set_source(m_background, x_offset, y_offset);
    */
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

    bool Panel::on_drawX(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        //  auto x_offset = 0, y_offset = 0;
        // Gdk::Rectangle background_rect = position::get_background_region();

        // cr->save();  // save the state of the context
        // cr->set_source_rgba(0.521, 0.6, 0, 1.0);
        // cr->paint();    // fill image with the color
        // cr->restore();  // color is back to black now*/

        cr->save();
        // cr->set_operator(Cairo::Operator::OPERATOR_CLEAR);
        //  cr->paint();
        cr->restore();

        if (!m_main_surface) {
            auto win = position::get_window();
            m_main_surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, win->get_width(),
                                                         win->get_height());
            //            m_main_surface.set_device_scale();
            // scale
            // cr->set_source(m_main_surface, x_offset, y_offset);
            //  m_main_surface.clear();
        }

        // composite dock layers and make sure to draw onto the window's context with one
        auto main_ctx = Cairo::Context::create(m_main_surface);
        main_ctx->set_operator(Cairo::Operator::OPERATOR_OVER);

        // draw background-layer
        // draw_background(main_ctx, background_rect, x_offset, y_offset);
        // draw_background(cr, background_rect, x_offset, y_offset);

        return true;
    }
    void Panel::draw_backgroundX(const Cairo::RefPtr<Cairo::Context>& cr,
                                 Gdk::Rectangle background_rect, int x_offset, int y_offset)
    {
        //  if (hide_progress > 0.0 && theme.CascadeHide) {
        int x = 2, y = 2;
        double hide_progress = 1.0;
        // position_manager.get_background_padding(out x, out y);
        x_offset -= (int)(x * hide_progress);
        y_offset -= (int)(y * hide_progress);
        //  }

        cr->set_source(m_background, background_rect.get_x() + x_offset,
                       background_rect.get_y() + y_offset);

        cr->paint();
        // cr->save();  // save the state of the context
        // cr->set_source_rgba(0.521, 0.6, 0, 1.0);
        // cr->paint();    // fill image with the color
        // cr->restore();  // color is back to black now
    }

}  // namespace docklight

