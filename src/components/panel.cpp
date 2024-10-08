// clang-format off
//#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/i18n.h>
#include <iostream>
#include <cmath>

#include "components/panel.h"
#include "translations.h"
#include "components/position.h"
// clang-format on
#include <cmath>
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
        g_message("Create Panel.");

        m_provider = create_provider();
    }

    void Panel::init()
    {
        m_sigc_updated =
            m_provider->signal_update().connect(sigc::mem_fun(this, &Panel::on_container_updated));

        m_position = Position();

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
        // m_sigc_draw =
        // Glib::signal_timeout().connect(sigc::mem_fun(this, &Panel::on_timeout_draw), 1000 / 30);

        m_mouse_enter = true;
        return false;
    }

    bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
    {
        // m_sigc_draw.disconnect();
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

    void Panel::container_updated(guint explicit_size) const
    {
        // TODO change after home icon is insertet, size will be == size - 1
        gint size = m_provider->data().size();
        if (explicit_size) size = explicit_size;
        if (!size) return;

        size--;
        int separator_size = Config()->get_separator_size();
        int separators_count = (size * separator_size);
        int required_size = m_provider->required_size(separators_count);

        // m_position->set_position(required_size);
        // return;

        auto workarea = device::monitor::get_workarea();
        // TODO Test
        // m_position->set_position(required_size);

        if (required_size + Config()->get_separator_size() > workarea.get_width()) {
            const int screen_width = workarea.get_width();  //
            //   required_size = workarea.get_width();           // - area * 2;

            // const int icon_size = Config()->get_icon_size();
            const int area = Config()->get_dock_area();
            const int icon_size = 128;  // area;  // Config()->get_icon_size();
            const int num_icons = size + 1;

            double scaling_factor = screen_width / (double)(num_icons * icon_size);
            // int scales_size = icon_size;
            //    scales_size *= scaling_factor;
            int scales_size = std::floor(icon_size * scaling_factor);

            int final_icon = icon_size - std::abs(scales_size - icon_size);
            if (final_icon > area) final_icon = area;
            if (final_icon <= 0) final_icon = 32;

            g_print(" ADD final-%d scaled------%d\n", final_icon, scales_size);
            g_print("screen wodth-------------%d\n", screen_width);
            g_print("size------------%d\n", num_icons);
            g_print("icon size-------------%d\n", icon_size);
            g_print("scales size-------------%d\n", scales_size);
            g_print("final size-------------%d\n", final_icon);
            g_print("\n\n\n\n\n\n\n\n\n\n\n\n\n");
            //    Config()->set_icon_size(std::abs(final_icon));
            // Config()->set_icon_size(std::abs(scales_size));

            if (required_size > workarea.get_width()) {
                //
                while (required_size > workarea.get_width()) {
                    g_print("Loop \n");
                    gint size = m_provider->data().size();
                    int separator_size = Config()->get_separator_size();
                    int separators_count = (size * separator_size);

                    auto icon_size = Config()->get_icon_size();
                    icon_size -= 2;
                    Config()->set_icon_size(std::abs(icon_size));

                    required_size = m_provider->required_size(separators_count);
                }
            }

            m_position->set_position(workarea.get_width());
            return;
            if (required_size > workarea.get_width()) {
                g_print(" RECALLLLLLLLLLLLLLLLLL-------------%d\n", final_icon);

                auto diff = required_size - workarea.get_width();
                g_print(" DIFF--%d cal %d\n", diff, 128 - diff);

                auto icon_size = Config()->get_icon_original_size();
                icon_size -= 8;

                // Config()->set_icon_size(std::abs(icon_size));
                Config()->set_icon_size(std::abs(final_icon));
                m_position->set_position(workarea.get_width());
            }

            //  m_position->force_position();

            // Gtk::Widget::queue_draw();
            g_print("screen wodth-------------%d\n", screen_width);
            g_print("size------------%d\n", num_icons);
            g_print("icon size-------------%d\n", icon_size);
            g_print("scales size-------------%d\n", scales_size);
            g_print("final size-------------%d\n", final_icon);
            g_print("\n\n\n\n\n\n\n\n\n\n\n\n\n");
            return;
        }

        Config()->set_icon_size(std::abs(128));
        m_position->set_position(required_size);
        if (required_size > workarea.get_width()) {
            m_position->set_position(workarea.get_width());
        }
        return;

        g_print("required size [%d] width: %d size: %d \n", required_size, workarea.get_width(),
                size);

        int current_icon_size = Config()->get_icon_original_size();
        // int current_icon_size = Config()->get_icon_size();
        int area = Config()->get_dock_area();
        int diff = required_size - workarea.get_width();

        // int diff = std::max(required_size, workarea.get_width()) -
        // std::min(required_size, workarea.get_width());

        //        int icon_size = diff / area;  // current_icon_size;
        int icon_size = area;
        //      current_icon_size -
        //      (Config()->get_icon_size() / (size + 1));  //)workarea.get_width() / (size + 1) -
        //      128;
        if (icon_size > 128) icon_size = 128;
        if (icon_size < 32) icon_size = 32;

        // separator_size = Config()->get_separator_original_size();
        // separator_size /= (size + 1) + separator_size;

        if (separator_size <= 0) separator_size = 128;
        ;  // Config()->get_separator_original_size();
        //        diff - current_icon_size;
        g_print(">>> OLD ICON SIZE %d   NEW ICON SIZE ==  %d \n", current_icon_size, icon_size);
        g_print(">>> OLD SEPARATOR_SIZE ==  %d \n", separator_size);
        // std::abs((int)current_icon_size - (int)icon_size);
        // icon_size -= std::abs((int)current_icon_size - (int)icon_size);
        if (diff < 128) {
            //            exit(1);
        }
        // icon_size -= (diff / size);
        if (required_size + area > (int)workarea.get_width()) {
            required_size = workarea.get_width();
            gint size = m_provider->data().size();
            // if (explicit_size) size = explicit_size;
            // if (!size) return;

            // size--;
            // int separator_size = Config()->get_separator_size();
            // int separators_count = (size * separator_size);
            // int required_size = m_provider->required_size(separators_count);

            g_print(">>> I N S D E!!!!!!!!!!!! TO BIG  DIFF %d\n", diff);
            // icon_size -= current_icon_size - (diff / size);
            // icon_size -= (diff / area);
            // icon_size -=
            //((required_size - diff) / (size * area));  // / area;  // area;  // / (size + 1)

            // icon_size -= std::abs((required_size - diff) / (size + 1));
            g_print(">>> !!!!!!!!!!!!  NEW ICON SIZE  %d %d\n", icon_size, required_size - diff);
            // / area;  // area;  // / (size + 1)

            //  icon_size += 10;  // Scurrent_icon_size;

            //  icon_size = 46;
            const int screen_width = required_size;  // workarea.get_width();  // - area * 2;
            //  const int icon_size = Config()->get_dock_area();
            // const int icon_size = Config()->get_icon_size();
            const int icon_size = 128;  // Config()->get_icon_size();
            const int num_icons = size + 1;

            double scaling_factor = screen_width / (double)(num_icons * icon_size);
            // double scaling_factor = (int)workarea.get_width() / ((int)size * (int)128);
            std::cout << "sice " << scaling_factor << std::endl;
            // double scaling_factor = 1.1842;
            //  `   icon_size = current_icon_size;
            // int aicon_size =
            // std::abs(current_icon_size - (current_icon_size * scaling_factor)) - 128;

            int isize = icon_size;
            isize *= scaling_factor;
            //  if (isize > 128) isize = 128;

            g_print("AREA %d\n", area);
            g_print("SCALING FACTOR %f\n", scaling_factor);
            g_print("ICON_SIZE %d\n", isize);
            g_print("ICON_SIZE  MINUS %d\n", std::abs(isize - icon_size));

            int xicon_size = icon_size - std::abs(isize - icon_size);
            g_print("ICON_SIZE RESULT %d\n", xicon_size);

            //            icon_size = aicon_size;

            Config()->set_icon_size(std::abs(xicon_size));
            // Config()->set_icon_size(std::abs(isize));

            // cal separato
            //
            //
            {
                const int screen_width = workarea.get_width();
                const int icon_size = Config()->get_separator_original_size();
                const int num_icons = size + 1;
                double scaling_factor = screen_width / (double)(num_icons * icon_size);

                g_print("SEPARA RESULT FACTOR %f\n", scaling_factor);

                int sepa = icon_size;
                sepa *= scaling_factor;

                g_print("SEPARA RESULT FACTOR %d\n", sepa);
                g_print("SEPARA RESULT FINAL %d\n", std::abs(128 - sepa));
                int sepa_size = std::abs(128 - sepa);
                //    Config()->set_separator_size(sepa_size);
            }
            m_position->set_position(required_size);
            g_print("\n\n\n\n\n\n\n\n\n\n\n\n\n");
            return;
            // g_print(">>> !!!!!!!!!!!! TO BIG  NEW ICON SIZE  %d\n", icon_size);

            // diff = std::abs(diff);
            // icon_size -= std::abs(diff - icon_size);

            // icon_size -= area;
            //.. current_icon_size, diff, icon_size);
            // if (size > 1) {
            // auto maxiconsize = (int)Config()->get_separator_original_size();
            // auto factor = (maxiconsize / size);
            // separator_size = 128 - factor;  // - separator_size);
            //// separator_size = icon_size;     // auto aseparator_size = 128 / size;
            ////  / size);

            // std::cout << separator_size << std::endl;
            //// separator_size =;
            // g_print("SEPARA %d\n", (int)separator_size);
            //}
        }
        ////
        //  Config()->set_separator_size(separator_size);
        g_print(">>> NEW ICON SIZE %d   D I F F  %d NEW SIZE=%d\n", icon_size, diff,
                current_icon_size - icon_size);

        g_print(">>> NEW SEPARATOR_SIZE ==  %d \n", Config()->get_separator_size());
        // Config()->set_icon_size(std::abs(icon_size));
        // diff = (int)((diff + area) / 32);
        // diff = (int)(area );
        //      auto icon_size =
        //            (int)std::fabs(current_icon_size - diff);  // Config()->get_icon_size();  // -
        //            diff;
        /*if ((int)(required_size) > (int)workarea.get_width()) {
            g_print(">>> TO BIG  \n");
            // required_size = workarea.get_width() - 64;
            // auto diff = required_size - workarea.get_width();
            // required_size = workarea.get_width() - 4;
            required_size -= diff;  // workarea.get_width() - 4;
            // m_provider->required_size(separators_count- diff);  // diff;
        }*/

        // g_print(">>> items count %d\n", size + 1);
        // g_print(">>> current_icon_size [%d] diff-factor :( %d )\n icon_size %d\n",
        // current_icon_size, diff, icon_size);
        Config()->set_icon_size(std::abs(icon_size));

        // separator_size -= std::abs(Config()->get_separator_original_size() - size);
        //        if (std::abs(Config()->get_separator_size() > 2))

        m_position->set_position(required_size);
        g_print("\n\n\n\n\n\n\n\n\n\n\n\n\n");
    }

    void Panel::on_container_updated(window_action_t action, int index)
    {
        container_updated();
        Gtk::Widget::queue_draw();
    }

    bool Panel::on_motion_notify_event(GdkEventMotion* event)
    {
        return false;
    }

    inline guint Panel::get_dockitem_index(int mx, int my)
    {
        gint pos_x = 0;
        gint pos_y = 0;

        auto separator_size = Config()->get_separator_size();
        auto area = Config()->get_dock_area() + separator_size;
        auto size = m_provider->data().size();
        auto maxsize = size * area;
        auto start_pos = 0;

        get_start_pos(maxsize, pos_x, pos_y);

        for (size_t idx = 0; idx < size; idx++) {
            m_dockitem_index = -1;
            if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                if (mx >= pos_x && mx <= pos_x + area) {
                    m_dockitem_index = idx;
                    break;
                }
                pos_x += start_pos + area;

            } else {  // Vertical
                if (my >= pos_y && my <= pos_y + area) {
                    m_dockitem_index = idx;
                    break;
                }
                pos_y += start_pos + area;
            }
        }

        return m_dockitem_index;
    }

    bool Panel::on_button_press_event(GdkEventButton* event)
    {
        m_dockitem_index = get_dockitem_index(event->x, event->y);

        std::shared_ptr<DockItemIcon> dockitem;
        if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;

        // g_print("%d x %d  index = %d %s\n", (int)event->x, (int)event->y, m_dockitem_index,
        // dockitem->to_string().c_str());
        //////////////////

        if (m_dockitem_index == 0) {
            auto iconsize = Config()->get_icon_size();
            iconsize -= 2;
            Config()->set_icon_size(iconsize);
            m_position->force_position();
            container_updated();
            Gtk::Widget::queue_draw();

            g_print("EXEC");
            g_print("index = %d\n%d\n", m_dockitem_index, iconsize);
        } else {
            m_position->reset_position();
        }

        //   Config()->set_icon_size(32);
        //   m_position->reset_position();

        return false;
    }

}  // namespace docklight

