#include "components/panel.h"
#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/i18n.h>
#include <glibmm/main.h>
#include <glibmm/timer.h>
#include "appwindow.h"
#include "components/config.h"
#include "utils/cairo.h"
#include "utils/easing.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/system.h"

#define DEF_ICONNAME "data/images/docklight.home.ico"

DL_NS_BEGIN

bool Panel::m_mouse_inside;

autohide_static_t Panel::m_autohide_static_type;
Panel* Panel::m_this;
bool Panel::m_visible;
WnckWindow* Panel::m_active_window;
Panel::Panel()
{
    m_app_updater.signal_update().connect(
        sigc::mem_fun(this, &Panel::on_appupdater_update));

    // Set event masks
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
               Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK |
               Gdk::POINTER_MOTION_HINT_MASK | Gdk::FOCUS_CHANGE_MASK |
               Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK |
               Gdk::POINTER_MOTION_HINT_MASK | Gdk::POINTER_MOTION_MASK);

    m_this = this;
    Panel::m_mouse_inside = false;
    Panel::m_visible = true;
    Panel::m_active_window = nullptr;

    //  Gets the default WnckScreen on the default display.
    WnckScreen* wnckscreen = wnck_screen_get_default();
    g_signal_connect(wnckscreen, "active_window_changed",
                     G_CALLBACK(Panel::on_active_window_changed), nullptr);
    g_signal_connect(wnckscreen, "active_workspace_changed",
                     G_CALLBACK(Panel::on_active_workspace_changed), nullptr);

    // https://developer.gnome.org/gtk-tutorial/stable/x334.html
    //

    // g_signal_handler_disconnect(wnckscreen, m_geometry_change);

    string filename(system_util::get_current_path(DEF_ICONNAME));
    appinfo_t info;
    auto icon_size = config::get_icon_size();
    auto const homePixbuf =
        pixbuf_util::get_from_file(filename, icon_size, icon_size);

    info.m_name = "desktop";
    info.m_title = _("Desktop");

    m_app_updater.m_dockitems.push_back(
        shared_ptr<DockItem>(new DockItem(info)));
    auto const new_item = m_app_updater.m_dockitems.back();
    new_item->set_image(homePixbuf);
}

void Panel::init()
{
    if (config::is_autohide()) {
        m_animation_hide_delay = 1.5;
        m_autohide_static_type.m_animation_state = DEF_HIDE;
        Panel::connect_autohide_signal(true);
        m_autohide_static_type.m_animation_timer.start();
    }
}

Panel::~Panel()
{
    g_print("Free Panel\n");
}

void Panel::connect_autohide_signal(bool connect)
{
    auto& s = Panel::m_autohide_static_type;
    if (connect) {
        if (!s.m_connect_autohide_signal_set) {
            s.m_sigc_autohide = Glib::signal_timeout().connect(
                sigc::mem_fun(*m_this, &Panel::auto_hide_timer), 33.33);

            s.m_connect_autohide_signal_set = true;
        }
    } else {
        s.m_sigc_autohide.disconnect();
        s.m_connect_autohide_signal_set = false;
    }

    if (s.m_connect_autohide_signal_set)
        g_print("Connected\n");
    else
        g_print("disconnect\n");
}

void Panel::connect_draw_signal(bool connect)
{
    if (connect) {
        if (!m_connect_draw_signal_set) {
            m_sigc_draw = Glib::signal_timeout().connect(
                sigc::mem_fun(*this, &Panel::on_timeout_draw), 125.0);

            m_connect_draw_signal_set = true;
        }
    } else {
        m_sigc_draw.disconnect();
        m_connect_draw_signal_set = false;
    }
}

void Panel::on_appupdater_update(bool a, int b)
{
    g_print("From panel Update signal resquested\n");
    AppWindow::update();

    m_draw_required = true;
    on_timeout_draw();
}

int Panel::get_required_size()
{
    int separators =
        config::get_separator_margin() * (m_app_updater.m_dockitems.size() - 1);
    return (m_app_updater.m_dockitems.size() * config::get_icon_size()) +
           config::get_window_start_end_margin() + separators;
}

bool Panel::on_timeout_draw()
{
    if (m_draw_required || Panel::m_mouse_inside) {
        Gtk::Widget::queue_draw();
        m_draw_required = false;
    }
    return true;
}

inline int Panel::get_index(const int& mouseX, const int& mouseY)
{
    Gdk::Point mouse(mouseX, mouseY);
    int idx = 0;
    int x = config::get_window_start_end_margin() / 2;
    int y = x;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        for (auto item : m_app_updater.m_dockitems) {
            if (mouse.get_x() >= x && mouse.get_x() <= x + item->get_width()) {
                return idx;
            }

            x += item->get_width() + config::get_separator_margin();
            idx++;
        }
    } else {
        int height;
        for (auto item : m_app_updater.m_dockitems) {
            height = item->get_dock_item_type() == dock_item_type_t::separator
                         ? item->get_width()
                         : item->get_height();

            if (mouse.get_y() >= y && mouse.get_y() <= y + height) {
                return idx;
            }

            y += height + config::get_separator_margin();
            idx++;
        }
    }

    return -1;
}

bool Panel::on_motion_notify_event(GdkEventMotion* event)
{
    //    g_print("Moution\n");

    m_current_index = this->get_index(event->x, event->y);
    /*
    if (m_DragDropBegin) {
        m_AppUpdater->MoveItem(this->m_currentMoveIndex);
        m_dragdropMousePoint.set_x((int)event->x);
        m_dragdropMousePoint.set_y((int)event->y);
    }*/

    //   if(DockPanel::m_previewIndex != m_currentMoveIndex &&
    //   DockPanel::m_dockPreview != nullptr){
    //       DockPanel::PreviewClose();
    //   }

    return false;
}

bool Panel::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    Panel::m_mouse_inside = true;

    // if (config::is_intelihide()) {
    // if (!m_visible) {
    // m_autohide_static_type.m_animation_state = DEF_SHOW;
    // Panel::connect_autohide_signal(true);
    //}
    //}
    if (config::is_autohide() || config::is_intelihide()) {
        if (!m_visible) {
            m_autohide_static_type.m_animation_state = DEF_SHOW;
            Panel::connect_autohide_signal(true);
        }
    }

    this->connect_draw_signal(true);
    // m_HomeMenu.hide();
    // m_ItemMenu.hide();

    return true;
}

bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_current_index = -1;
    on_timeout_draw();

    Panel::m_mouse_inside = false;

    if (config::is_intelihide()) {
        Panel::connect_autohide_signal(false);
        Panel::check_intelihide();
    }

    if (config::is_autohide()) {
        m_autohide_static_type.m_animation_state = DEF_HIDE;
        Panel::connect_autohide_signal(true);
        m_autohide_static_type.m_animation_timer.start();
    }

    this->connect_draw_signal(false);
    // m_titlewindow.hide();
    // m_infowindow.hide();

    // if (m_popupMenuOn || m_dragDropItem) {
    // return true;
    //}

    // DockPanel::set_SelectorForActiveWindow(nullptr);
    return true;
}

void Panel::on_geometry_change(WnckWindow* window, gpointer user_data)
{
    if (!config::is_intelihide()) {
        return;
    }

    Panel::check_intelihide();
}

void Panel::check_intelihide()
{
    if (m_active_window == nullptr || config::is_intelihide() == false) {
        return;
    }

    int xp = 0, yp = 0, widthp = 0, heightp = 0;
    wnck_window_get_geometry(m_active_window, &xp, &yp, &widthp, &heightp);

    Gdk::Rectangle rect_dock = position_util::get_appwindow_geometry();
    const Gdk::Rectangle rect_window(xp, yp, widthp, heightp);

    if (rect_dock.intersects(rect_window)) {
        m_autohide_static_type.m_animation_timer.start();
        if (m_visible) {
            m_autohide_static_type.m_animation_state = DEF_HIDE;
            Panel::connect_autohide_signal(true);
        }
    } else {
        if (!m_visible) {
            m_autohide_static_type.m_animation_state = DEF_SHOW;
            Panel::connect_autohide_signal(true);
        }
    }
}

void Panel::on_active_workspace_changed(WnckScreen* screen,
                                        WnckWorkspace* previously_active_space,
                                        gpointer user_data)
{
    if (config::is_intelihide()) {
        Panel::check_intelihide();
    }
}

void Panel::on_active_window_changed(WnckScreen* screen,
                                     WnckWindow* previously_active_window,
                                     gpointer user_data)
{
    WnckWindow* active_window = wnck_screen_get_active_window(screen);
    if (!active_window) {
        return;
    }

    Panel::m_active_window = active_window;

    if (config::is_intelihide()) {
        Panel::check_intelihide();

        if (previously_active_window != nullptr &&
            m_autohide_static_type.m_geometry_change_id != 0) {
            g_signal_handler_disconnect(
                previously_active_window,
                m_autohide_static_type.m_geometry_change_id);
        }
        m_autohide_static_type.m_geometry_change_id =
            g_signal_connect(active_window, "geometry-changed",
                             G_CALLBACK(Panel::on_geometry_change), nullptr);
    }
}

inline void Panel::get_item_position(const dock_item_type_t item_typ, int& x,
                                     int& y, int& width, int& height)
{
    static int nextsize = 0;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        if (x == 0) {
            y = 4;
            x = config::get_window_start_end_margin() / 2;
            nextsize = width;
            return;
        }

        x += nextsize + config::get_separator_margin();
        nextsize = width;
    } else {
        if (y == 0) {
            x = 4;
            y = config::get_window_start_end_margin() / 2;
            nextsize = height;
            return;
        }

        // if the item is a separator the wdth is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        // if (dockType == DockItemType::Separator) {
        // y += nextsize + Configuration::get_separatorMargin();
        // height = nextsize = width;
        // width = Configuration::get_CellWidth();
        // return;
        //}

        y += nextsize + config::get_separator_margin();
        nextsize = height;
    }
    return;

    /*if (DockWindow::is_Horizontal()) {
        if (x == 0) {
            y = DockWindow::get_DockWindowHeight() / 2 - height / 2;
            x = DockWindow::get_dockWindowStartEndMargin() / 2;
            nextsize = width;
            return;
        }

        // if the item is a separator the width is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        if (dockType == DockItemType::Separator) {
            x += nextsize + Configuration::get_separatorMargin();
            nextsize = width;
            return;
        }

        x += nextsize + Configuration::get_separatorMargin();
        nextsize = width;
    } else {
        if (y == 0) {
            x = Configuration::get_dockWindowSize() / 2 - width / 2;
            y = DockWindow::get_dockWindowStartEndMargin() / 2;
            nextsize = height;
            return;
        }

        // if the item is a separator the wdth is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        if (dockType == DockItemType::Separator) {
            y += nextsize + Configuration::get_separatorMargin();
            height = nextsize = width;
            width = Configuration::get_CellWidth();
            return;
        }

        y += nextsize + Configuration::get_separatorMargin();
        nextsize = height;
    }*/
}

bool Panel::intelli_hide_timer()
{
    return true;
}

float m_easing_duration = 6.0;
float m_initTime = 0;
int m_offset_x = 0;
int m_offset_y = 0;
bool Panel::auto_hide_timer()
{
    if (config::is_autohide() == false && config::is_intelihide() == false) {
        return false;
    }
    g_print("anim %d %f v:%d\n", m_autohide_static_type.m_animation_state,
            m_autohide_static_type.m_animation_timer.elapsed(), (int)m_visible);

    if (m_autohide_static_type.m_animation_state == DEF_HIDE && m_visible &&
        !m_mouse_inside && !m_animation_running &&
        abs(m_autohide_static_type.m_animation_timer.elapsed()) >
            m_animation_hide_delay) {
        g_print("start hide\n");
        m_easing_duration = 5.0;
        m_animation_running = true;
    }

    if (m_autohide_static_type.m_animation_state == DEF_SHOW && !m_visible &&
        /* m_mouse_inside &&*/ !m_animation_running) {
        g_print("start show\n");
        m_easing_duration = 4.0;
        m_animation_running = true;
    }

    if (m_animation_running) {
        auto endTime = m_initTime + m_easing_duration;
        float startPosition = 0;
        float endPosition = 0;

        auto location = config::get_dock_location();
        switch (location) {
            case dock_location_t::left:
            case dock_location_t::top: {
                if (m_visible) {
                    startPosition = 0;
                    endPosition = -(config::get_dock_area() + 1);
                } else {
                    startPosition = -(config::get_dock_area() + 1);
                    endPosition = 0;
                }
                break;
            }
            case dock_location_t::right:
            case dock_location_t::bottom: {
                if (m_visible) {
                    startPosition = 0;
                    endPosition = config::get_dock_area();
                } else {
                    startPosition = config::get_dock_area();
                    endPosition = 0;
                }
                break;
            }
        }

        float position = easing_util::map_clamp(
            m_animation_time, m_initTime, endTime, startPosition, endPosition,
            &easing_util::linear::easeIn);

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            m_offset_x = 0;
            m_offset_y = (int)position;
        } else {
            m_offset_x = (int)position;
            m_offset_y = 0;
        }

        // g_print("anim %d\n", (int)position);

        Gtk::Widget::queue_draw();
        m_animation_time++;

        if ((int)position == (int)endPosition) {
            m_autohide_static_type.m_animation_timer.stop();
            m_autohide_static_type.m_animation_timer.reset();
            m_animation_running = false;
            m_animation_time = 0;

            m_visible = (int)endPosition == 0;
            Panel::connect_autohide_signal(false);
            //    config::get_dock_area());
        }
    }

    return true;
}
bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    this->draw_panel(cr);
    this->draw_items(cr);
    return true;
}
void Panel::draw_panel(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Gdk::Rectangle rect = position_util::get_appwindow_geometry();
    rect.set_x(m_offset_x);
    rect.set_y(m_offset_y);
    cairo_util::rounded_rectangle(cr, rect.get_x(), rect.get_y(),
                                  rect.get_width(), rect.get_height(), 3);

    cr->set_source_rgba(0.8, 0.8, 0.8, 1.0);
    cr->fill();
    //   cr->paint();
}

void Panel::draw_items(const Cairo::RefPtr<Cairo::Context>& cr)
{
    int y = 0;
    int x = 0;
    int width = 0;
    int height = 0;
    int center = 0;
    int area = config::get_dock_area();

    size_t items_count = m_app_updater.m_dockitems.size();

    // Draw all items with cairo
    for (size_t idx = 0; idx < items_count; idx++) {
        auto const item = m_app_updater.m_dockitems[idx];

        width = item->get_width();
        height = item->get_height();

        this->get_item_position(dock_item_type_t::single, x, y, width, height);

        //     g_print("Draw %d\n", x);
        item->set_index(idx);
        item->set_x(x);
        item->set_y(y);

        // draw cell
        cr->set_source_rgba(0, 0, 1, 1);
        cairo_util::rounded_rectangle(cr, m_offset_x + x, m_offset_y + y, width,
                                      height, 3);
        cr->stroke();

        // draw dots
        center = (width / 2);
        if (item->m_items.size() > 0) {
            cr->set_source_rgb(1.0, 1.0, 1.0);
            if (item->m_items.size() == 1) {
                cr->arc(m_offset_x + x + center, m_offset_y + y + area - 5, 1.7,
                        0, 2 * M_PI);
            } else if (item->m_items.size() > 1) {
                cr->arc(m_offset_x + x + center - 3, m_offset_y + y + area - 5,
                        1.7, 0, 2 * M_PI);
                cr->arc(m_offset_x + x + center + 3, m_offset_y + y + area - 5,
                        1.7, 0, 2 * M_PI);
            }
            cr->fill();
        }

        // draw icon
        // auto const image =
        // item->get_image()->scale_simple(48, 48, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(cr, item->get_image(), m_offset_x + x,
                                      m_offset_y + y);
        cr->paint();

        // draw selector
        if ((int)idx == m_current_index) {
            auto tmp = item->get_image()->copy();
            pixbuf_util::invert_pixels(tmp);
            Gdk::Cairo::set_source_pixbuf(cr, tmp, m_offset_x + x,
                                          m_offset_y + y);
            cr->paint();

            // cr->set_source_rgba(1, 1, 1, 0.4);
            // cairo_util::rounded_rectangle(cr, x, y, width, height, 0);
            // cr->fill();
        }
    }
}

DL_NS_END
