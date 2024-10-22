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

    void Panel::init(Glib::RefPtr<Gtk::Application> app)
    {
        m_app = app;
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
        // Glib::signal_timeout().connect(sigc::mem_fun(this, &Panel::on_timeout_draw), 1000 / 9);

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

    void Panel::on_home_menu_quit_event()
    {
        m_app->quit();
    }

    void Panel::container_updated(guint explicit_size)
    {
        auto size = m_provider->data().size();  //- 1;
        auto separator_size = Config()->get_separator_size();
        auto separators_count = (size * separator_size);

        // resize the icon if necesery
        int scaled_icon_size = this->get_scale_factor();
        Config()->set_icon_size(scaled_icon_size);

        // draw and positioning
        auto required_size = m_provider->required_size(separators_count);
        m_position->set_position(required_size);
    }

    void Panel::on_container_updated(window_action_t action, int index)
    {
        container_updated();
        Gtk::Widget::queue_draw();
    }

    bool Panel::on_motion_notify_event(GdkEventMotion* event)
    {
        get_dockitem_index(event->x, event->y);

        if (m_mouse_enter) {
            //      Gtk::Widget::queue_draw();
        }

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

    bool Panel::on_scroll_event(GdkEventScroll* e)
    {
        if (m_dockitem_index < 1) return false;

        std::shared_ptr<DockItemIcon> dockitem;
        if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;

        auto size = dockitem->get_childmap().size();
        if (!size) return false;

        static size_t idx = 0;

        if (idx++ >= size - 1) idx = 0;
        auto it = dockitem->get_childmap().begin();
        std::advance(it, idx);
        auto child = it->second;

        WnckWindow* window = child->get_wnckwindow();
        if (!window) return false;

        wnck::activate_window(window);

        return false;
    }

    void Panel::on_item_menu_childlist_event(WnckWindow* window)
    {
        wnck::activate_window(window);
    }

    bool Panel::on_button_press_event(GdkEventButton* event)
    {
        if ((event->type != GDK_BUTTON_PRESS)) return false;

        get_dockitem_index(event->x, event->y);

        std::shared_ptr<DockItemIcon> dockitem;
        if (!m_provider->get_dockitem_by_index(m_dockitem_index, dockitem)) return false;

        if (event->button == 1 && m_dockitem_index > 0) {
            auto size = dockitem->get_childmap().size();
            if (!size) {
                dockitem->launch();
                return false;
            }

            auto it = dockitem->get_childmap().begin();
            std::advance(it, 0);
            auto child = it->second;

            WnckWindow* window = child->get_wnckwindow();
            if (!window) {
                return false;
            }

            wnck::activate_window(window);
            // show group
        } else if (event->button == 3) {
            if (m_dockitem_index == 0) {
                // Home Menu
                if (!m_home_menu.get_attach_widget()) {
                    m_home_menu.attach_to_widget(*this);
                }

                m_home_menu.popup(sigc::mem_fun(*this, &Panel::on_home_menu_position), 1,
                                  event->time);

            } else if (m_dockitem_index > 0) {
                // Items Menu
                m_item_menu_attach.set_active(dockitem->get_attached());

                // populate childrens;
                static Gtk::SeparatorMenuItem* separator = nullptr;
                for (auto& itemMenu : m_item_menu.get_children()) {
                    auto type = dynamic_cast<Gtk::ImageMenuItem*>(itemMenu);
                    if (type) {
                        m_item_menu.remove(*itemMenu);
                    }
                }

                m_item_menu.show_all();
                if (separator) m_item_menu.remove(*separator);

                separator = Gtk::manage(new Gtk::SeparatorMenuItem());
                m_item_menu.append(*separator);
                // m_item_menu.insert(*separator, 0);

                for (auto& item : dockitem->get_childmap()) {
                    auto child = item.second;

                    Gtk::ImageMenuItem* menu_item =
                        Gtk::manage(new Gtk::ImageMenuItem(child->get_title()));

                    const Glib::RefPtr<Gdk::Pixbuf> pixbuf = dockitem->get_icon(16);
                    Gtk::Image* image = Gtk::manage(new Gtk::Image(pixbuf));

                    menu_item->set_image(*image);
                    menu_item->set_always_show_image(true);
                    menu_item->set_label(child->get_window_name());

                    menu_item->signal_activate().connect(sigc::bind<WnckWindow*>(
                        sigc::mem_fun(*this, &Panel::on_item_menu_childlist_event),
                        child->get_wnckwindow()));

                    m_item_menu.append(*menu_item);
                    // m_item_menu.insert(*menu_item, 0);
                }

                m_item_menu.show_all();
                m_item_menu.popup_at_pointer(nullptr);
                m_item_menu.popup(sigc::mem_fun(*this, &Panel::on_item_menu_position), 0,
                                  event->time);
            }
        }

        // It has been handled.
        return true;
        ////////////////////
        ///

        if (!dockitem->get_childmap().size()) return false;

        dockitem->set_attached();
        m_provider->save();
        //        std::shared_ptr<DockItemIcon> child = dockitem->get_childmap().at(0);
        auto it = dockitem->get_childmap().begin();
        std::advance(it, 0);
        auto child = it->second;
        auto window = child->get_wnckwindow();
        if (window) wnck::activate_window(window);
        // auto child = item.second;

        // WnckWindow* window = child->get_wnckwindow();
        //  wnck::activate_window(window);

        return false;
        //  container_updated();
        //  return false;

        // ..    container_updated();
        //     m_provider->remove(0);
        g_print("ATTACHED %s\n", dockitem->to_string().c_str());
        //   m_provider->load();
        for (auto& dockitem : m_provider->data()) {
            g_print("X %ld %s attach %d child %ld\n", dockitem->get_xid(),
                    dockitem->get_group_name().c_str(), (int)dockitem->get_attached(),
                    dockitem->get_childmap().size());
            for (auto& item : dockitem->get_childmap()) {
                auto child = item.second;

                WnckWindow* window = child->get_wnckwindow();

                g_print("   -| %ld %s attach %d wnck %p\n", child->get_xid(),
                        child->get_group_name().c_str(), 0, &window);
                break;
            }
        }

        // launcher2(dockitem->get_desktop_file(), dockitem->get_instance_name(),
        // dockitem->get_group_name(), dockitem->get_icon_name());
        return false;
    }

}  // namespace docklight

