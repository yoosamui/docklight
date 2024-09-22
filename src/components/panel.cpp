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

#define DOCK_MARGIN_SPACE 12
#define DOCK_iCON_SIZE 64

namespace docklight
{

    namespace cairo
    {

        /* Glib::RefPtr<Surface>*/ void CreateSurface(int width, int height)

        {
            Surface obj;
            //   Glib::RefPtr<Surface> ss = Glib::RefPtr<Surface>(new Surface());

            // obj->value = 2l;
            // auto c = ss->get_width();

            // auto a = obj->get_width();

            ////  cairo_surface_t * cobject);  //: Cairo::ImageSurface(cobject)
            //   auto t = Glib::RefPtr<Surface>(new Surface(ck));
            //   t->get_width();
            //   int a = t.value;

            //// return Glib::RefPtr<Surface>(new Surface(width, height));
        }

    }  // namespace cairo

    gdouble rotation = 0;
    double DegreesToRadians(int degrees);

    double DegreesToRadians(int degrees)
    {
        return ((double)((double)degrees * (M_PI / 180)));
    }

    gboolean rotate_cb(void* degrees)
    {
        // Any rotation applied to cr here will be lost, as we create
        // a new cairo context on every expose eventm_cell
        // cairo_rotate (cr, 4);
        rotation += DegreesToRadians((*(int*)(degrees)));
        // cairo_paint(cr);
        //       printf("rotating\n");
        //  Tell our window that it should repaint itself (ie. emit an expose event)
        //  gtk_widget_queue_draw(window);

        return (TRUE);
    }
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
        m_sigc_updated = get_dockcontainer()->signal_update().connect(
            sigc::mem_fun(this, &Panel::on_container_updated));

        // frame_time = GLib.get_monotonic_time();
        // m_app_provider = Glib::RefPtr<AppProvider>(new AppProvider());

        // DockItem* m_dockitem = DockItem::getInstance();

        //..const auto dockitem = DockItemProvider::create();
        //  dockitem->get_childrens();
    }

    Panel::~Panel()
    {
        m_sigc_updated.disconnect();

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
        auto home_pixbuf = Gdk::Pixbuf::create_from_file(iconname, size, size);\h
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

    const Cairo::RefPtr<Cairo::ImageSurface> Panel::create_iconX(const Glib::RefPtr<DockItem>& item)
    {
        if (!m_icon_obj->surface) {
            m_icon_obj.create(64, 64);
        }
        auto ctx = m_icon_obj.ctx();  // )context;
        ctx->set_operator(Cairo::Operator::OPERATOR_CLEAR);
        // ctx->save();
        auto icon = item->get_icon()->scale_simple(64, 64, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(ctx, icon, 0, 0);

        //   ctx->set_operator(Cairo::Operator::OPERATOR_CLEAR);
        ctx->paint();
        // ctx->restore();

        // iCairo::RefPtr<Cairo::Context> bck_ctx = Cairo::Context::create(m_background);
        auto bck_ctx = m_back_obj.ctx();  // context;
        //.Cairo::Context::create(m_background);
        bck_ctx->set_source(m_icon_obj->surface, 0, 0);
        bck_ctx->paint();

        return m_icon_obj->surface;
    }

    void Panel::draw_cell()
    {
        if (!m_cell) {
            int size = config::get_dock_area();
            m_cell = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        }

        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_cell);
        // clear
        ctx->save();
        ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);
        ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        ctx->paint_with_alpha(1.0);

        // Surface rect TEST
        ctx->set_line_width(2.0);
        ctx->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        ctx->rectangle(0, 0, m_cell->get_width(), m_cell->get_height());
        ctx->stroke();

        ctx->restore();

        // add to back surface
        // Cairo::RefPtr<Cairo::Context> bck_ctx = Cairo::Context::create(m_background);
        // bck_ctx->set_source(m_cell, m_posX, m_posY);
        // bck_ctx->paint();
    }
    void Panel::draw_icon()
    {
        int size = config::get_icon_size();

        if (!m_surfaceIcon) {
            //
            m_surfaceIcon = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        }
        std::string iconname("data/images/docklight.home.ico");
        auto home_pixbuf = Gdk::Pixbuf::create_from_file(iconname, size, size);
        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_surfaceIcon);

        Gdk::Cairo::set_source_pixbuf(ctx, home_pixbuf, 0, 0);
        ctx->paint();

        /*double draw_value_darken = 0.8;
        ctx->rectangle(0, 0, m_surfaceIcon->get_width(), m_surfaceIcon->get_height());
        ctx->set_source_rgba(0, 0, 0, draw_value_darken);
        ctx->set_operator(Cairo::Operator::OPERATOR_ATOP);
        // icon_cr.set_source_rgba(0, 0, 0, draw_value.darken);
        //   icon_cr.set_operator(Cairo.Operator.ATOP);
        ctx->fill();
        ctx->set_operator(Cairo::Operator::OPERATOR_OVER);*/
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
            // m_background = theme.create_background(background_rect.width,
            // background_rect.height, position_manager.Position, main_buffer);
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

    void Panel::on_container_updated(window_action_t action, int index)
    {
        g_message("container Updated\n");

        Gtk::Widget::queue_draw();
    }

    bool Panel::on_motion_notify_event(GdkEventMotion* event)
    {
        //   g_print("motion %d x %d\n", (int)event->x, (int)event->y);

        return false;
    }

    bool Panel::on_button_press_event(GdkEventButton* event)
    {
        if ((event->type == GDK_BUTTON_PRESS)) {
            //  g_print("on press %d x %d\n", (int)event->x, (int)event->y);

            /*Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
            // ctx->cairo_translate(64, 64);
            ctx->translate(64, 64);
            // cairo_rotate(cr, rotation);
            //
            rotation = DegreesToRadians(0);
            // rotate_cb(90);
            ctx->rotate(rotation);
            Gtk::Widget::queue_draw();

            g_print("rotate\n");*/
            return false;

            // m_cell = Glib::RefPtr<DockCell>(new DockCell());
            // m_cell->set_name("DOCKCELL");
            // m_cell->set_size_request(200, 200);
            // m_cell->show();

            /*m_cell->set_visible(true);
            m_cell->activate();
            int width, height;

            auto parent = m_cell->get_parent();
            if (parent) g_print("NAME %s\n", parent->get_name().c_str());

            m_cell->get_size_request(width, height);
            g_print("SIZE %d x %d\n", width, height);
            //  m_cell->move(800, 600);
            m_cell->show();*/
        }
        return true;
    }

    const Cairo::RefPtr<Cairo::ImageSurface> Panel::draw_icon(const Glib::RefPtr<DockItem>& item)
    {
        int size = config::get_icon_size();
        g_assert(m_background);

        //  if (!m_surfaceIcon) {
        m_surfaceIcon = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
        // }

        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_surfaceIcon);
        // clear
        ctx->save();
        ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);
        // ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        //... ctx->rectangle(0, 0, 64, 64);
        ctx->paint_with_alpha(1.0);
        // TODO check scale if do when no sizes changed. take care of speed
        auto icon = item->get_icon()->scale_simple(size, size, Gdk::INTERP_BILINEAR);

        // Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
        //  ctx->cairo_translate(64, 64);
        ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);

        // rotation test
        // ctx->translate(0, 64);
        // rotation = DegreesToRadians(90);
        // ctx->rotate(rotation);
        // paint surface rec
        ctx->set_line_width(2.0);
        ctx->set_source_rgb(0.0, 0.0, 0.0);
        ctx->rectangle(0, 0, m_surfaceIcon->get_width(), m_surfaceIcon->get_height());
        ctx->stroke();

        Gdk::Cairo::set_source_pixbuf(ctx, icon, 0, 0);
        ctx->paint();

        ctx->restore();

        // add to back cell surface
        Cairo::RefPtr<Cairo::Context> cell_ctx = Cairo::Context::create(m_cell);
        // ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        // cell_ctx->set_operator(Cairo::Operator::OPERATOR_DEST);
        cell_ctx->set_source(m_surfaceIcon, 0, 0);
        cell_ctx->paint();

        // add to back surface
        // Cairo::RefPtr<Cairo::Context> bck_ctx = Cairo::Context::create(m_background);
        // bck_ctx->set_source(m_cell, m_posX, m_posY);
        // bck_ctx->paint();
        return m_surfaceIcon;
    }

    const Cairo::RefPtr<Cairo::ImageSurface> Panel::draw_indicator(
        const Glib::RefPtr<DockItem>& item)
    {
        g_assert(m_background);

        int size = config::get_dock_area() - config::get_icon_size();
        int area = config::get_icon_size();

        if (!m_indicator) {
            m_indicator = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, area,
                                                      size);  // TODO: fix this think!
        }

        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_indicator);
        // clear
        ctx->save();
        ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);
        ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        ctx->paint_with_alpha(1.0);

        // Surface rect TEST
        // ctx->set_line_width(2.0);
        // ctx->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        // ctx->rectangle(0, 0, area, size);
        // ctx->stroke();

        ctx->restore();
        // ctx->set_line_width(2.0);
        ctx->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        int resize_factor = config::get_dock_area() / 16;
        if (item->get_childmap().size() == 1) {
            //           .. ctx->rectangle(0, 0, m_indicator->get_height(),
            //           m_indicator->get_width());
            // ctx->rectangle(0, m_indicator->get_height() - 4, m_indicator->get_width(),
            // resize_factor);

            ctx->rectangle(0, m_indicator->get_height() - resize_factor, m_indicator->get_width(),
                           resize_factor);

        } else {
            ctx->rectangle(0, m_indicator->get_height() - resize_factor,
                           m_indicator->get_width() / 2 - 4, resize_factor);

            ctx->rectangle(m_indicator->get_width() / 2 + 4,
                           m_indicator->get_height() - resize_factor, m_indicator->get_width() / 2,
                           4);
            // ctx->rectangle(0, 2, (64 / 2) - 4, 2);
            // ctx->rectangle((64 / 2) + 4, 2, (64 / 2) - 4, 2);
        }
        ctx->fill();

        // add to back surface
        Cairo::RefPtr<Cairo::Context> bck_ctx = Cairo::Context::create(m_background);

        // bck_ctx->set_source(m_indicator, m_posX, config::get_dock_area() - size);

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            bck_ctx->set_source(m_indicator, m_posX, config::get_dock_area() - size);
            //    bck_ctx->set_source(m_indicator, m_posX, m_surfaceIcon->get_width());
            // bck_ctx->set_source(m_indicator, m_posX, config::get_dock_area() - size);

            // m_posY += config::get_icon_size() + config::get_separator_margin();

        } else {
            bck_ctx->set_source(m_indicator, 0, config::get_dock_area() - size + m_posY);
            // bck_ctx->set_source(m_indicator, m_posX, m_surfaceIcon->get_width());
            //  bck_ctx->set_source(m_indicator, 0, (config::get_dock_area() + (m_posY - 8)));
            //  bck_ctx->set_source(m_indicator, 0, config::get_dock_area() + (m_posY - 8));
        }
        bck_ctx->paint();

        return m_indicator;
    }

    void Panel::draw_background()
    {
        if (!m_background) {
            Gdk::Rectangle bckrect = position::get_background_region();
            m_background = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, bckrect.get_width(),
                                                       bckrect.get_height());
        }

        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
        ctx->set_source_rgba(0.521, 0.6, 0, 1.0);
        ctx->paint();
    }

    //  bck_ctx->set_source_rgba(0.521, 0.6, 0, 1.0);
    bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
    {
        // if (!m_back_obj->surface) {
        // draw_background();
        //}
        if (!m_background) {
            draw_background();
        }

        //  m_back_obj->context->set_operator(Cairo::Operator::OPERATOR_CLEAR);
        auto container = get_dockcontainer();
        auto appmap = container->get_appmap();

        m_posX = 0;
        m_posY = 0;

        draw_cell();
        Cairo::RefPtr<Cairo::Context> bck_ctx = Cairo::Context::create(m_background);
        Cairo::RefPtr<Cairo::Context> cell_ctx = Cairo::Context::create(m_cell);
        // bck_ctx->set_operator(Cairo::Operator::OPERATOR_DEST);
        //  bck_ctx->set_operator(Cairo::Operator::OPERATOR_IN);
        //  bck_ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
        draw_background();

        guint tag = 0;
        g_print("LOOP %ld \n", appmap.size());
        for (auto it = appmap.begin(); it != appmap.end(); it++) {
            auto dockitem = it->second;
            dockitem->set_tag(tag++);

            // g_print("%d\n", m_posX);
            // Cairo::RefPtr<Cairo::Context> cell_ctx = Cairo::Context::create(m_cell);
            // cell_ctx->set_source_rgba(0.0, 0.0, 0.0, 0.0);
            ////  cell_ctx->set_operator(Cairo::Operator::OPERATOR_SOURCE);
            ////... ctx->rectangle(0, 0, 64, 64);
            // cell_ctx->paint_with_alpha(1.0);
            //
            draw_cell();
            draw_icon(dockitem);
            //  draw_indicator(dockitem);

            // add surfaceicon to the cell surface
            cell_ctx->set_source(m_surfaceIcon, m_posX, m_posY);
            cell_ctx->paint();

            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                m_posX += config::get_icon_size() + config::get_separator_margin();
            } else {
                m_posY += config::get_icon_size() + config::get_separator_margin() +
                          config::get_separator_margin();
            }
        }

        // add cell surfaceicon to the background surface
        bck_ctx->set_source(m_cell, m_posX, m_posY);
        bck_ctx->paint();

        cr->set_source(m_background, 0, 0);
        cr->paint();

        return false;

#ifdef DRAWX
        ;
        if (!m_background) {
            // auto win = position::get_window();
            // m_background = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,
            // win->get_width(), win->get_height()); draw_icon(); aplay_layers();
            //  g_message("%d x %d \n", win->get_width(), win->get_height());

            // create the surface;
            draw_background();

            // draw_icon();

            // draw_glow();

            // aplay_layers();
        }

        Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);
        ctx->rectangle(0, 0, m_background->get_width(), m_background->get_height());
        ctx->set_source_rgba(0.521, 0.6, 0, 1.0);
        // ctx->rectangle(0, 0, 64,
        //                64);  // m_background->get_width(), m_background->get_height());
        //  ctx->set_operator(Cairo::Operator::OPERATOR_CLEAR);
        ctx->fill();

        //  ctx->paint);

        auto container = get_dockcontainer();
        auto appmap = container->get_appmap();

        m_posX = 0;
        m_posY = 0;
        //        g_print("Draw!!!!!!!!!! count %d \n", (int)appmap.size());

        for (auto it = appmap.begin(); it != appmap.end(); it++) {
            auto dockitem = it->second;
            auto icon_surface = create_icon(dockitem);

            Cairo::RefPtr<Cairo::Context> bck_ctx = Cairo::Context::create(m_background);
            bck_ctx->set_source(icon_surface, m_posX, m_posY);
            bck_ctx->paint();
            //

            m_posX += 64 + 8;  // separator;

            //    g_print(" posX = %d\n", m_posX);
        }

        cr->set_source(m_background, 0, 0);
        cr->paint();
        return true;

        // auto background_rect = position::get_background_region();
        // draw_background(cr, background_rect, 0, m_posY);
        //  int x_offset = 0, y_offset = 0;
        //  Cairo::RefPtr<Cairo::Context> ctx = Cairo::Context::create(m_background);

        // ctx->set_source_rgba(1, 1, 1, 1.0);
        // ctx->paint();

        cr->set_source(m_background, 0, 30);
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
#endif
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

