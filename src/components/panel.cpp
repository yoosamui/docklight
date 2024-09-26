// clang-format off
//#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/i18n.h>
#include <iostream>
#include <cmath>

#include "components/panel.h"
#include "translations.h"
#include "components/position.h"
// clang-format on

namespace docklight
{
    // TODO move to somme tools.
    /*gdouble rotation = 0;
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
    */
    Panel::Panel()
    {
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
    }

    Panel::~Panel()
    {
        m_sigc_updated.disconnect();

        g_print(MSG_FREE_OBJECT, "Panel");
        g_print("\n");
    }
    bool Panel::on_enter_notify_event(GdkEventCrossing* crossing_event)
    {
        m_sigc_draw =
            Glib::signal_timeout().connect(sigc::mem_fun(this, &Panel::on_timeout_draw), 1000 / 30);

        m_mouse_enter = true;
        return false;
    }

    bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
    {
        m_sigc_draw.disconnect();
        m_mouse_enter = false;

        return false;
    }
    bool Panel::on_timeout_draw()
    {
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

        // if (m_animation_time < 1.0f) {
        // m_frame_time = g_get_real_time();
        // m_initTime = 0.0f;  // g_get_real_time();
        // m_endTime = 10.0f;  // m_initTime + 10.0f;
        //// auto endTime = m_initTime + 12;  // m_easing_duration;
        //// float startPosition = 0.f;
        //// float endPosition = 0.f;
        ////  m_animation_time = 0;
        //}

        // float pos = 0.0f;
        // float startPosition = 0.f;
        // float endPosition = 74.f;
        //// if (!nearly_equal(m_animation_time, endPosition, 2)) {
        // m_posY = easing::map_clamp(m_animation_time, m_initTime, m_endTime, startPosition,
        // endPosition, &easing::bounce::easeOut);

        // if (m_posY != endPosition) {
        //// std::cout << pos << ",  " << endPosition << std::endl;
        // g_print("%d\n", (int)m_posY);

        // m_animation_time += 0.4f;  // g_get_real_time() - m_frame_time;
        //} else {
        // m_posY = 0;
        // m_animation_time = 0.f;
        //}

        //// easing_util::map_clamp(m_animation_time, m_initTime, endTime, startPosition,
        ////                                   endPosition, &easing_util::linear::easeOut);
        ////  GLib.get_monotonic_time();
        ////    initialize_frame(frame_time);
        ////    widget widget.queue_draw();

        // return true;
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

    void Panel::container_updated() const
    {
        auto container = get_dockcontainer();
        // TODO change after home icon is insertet, will be == 1
        if (container->get_map().size() == 0) return;

        gint size = container->get_map().size() - 1;
        gint separator_size = config::get_separator_size();
        gint separators_count = (size * separator_size);

        position::set_window_position(container->required_size(separators_count));
    }

    void Panel::on_container_updated(window_action_t action, int index)
    {
        container_updated();
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

}  // namespace docklight

