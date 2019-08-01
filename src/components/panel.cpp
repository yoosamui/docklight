#include "components/panel.h"
#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/i18n.h>
#include <glibmm/main.h>
#include <glibmm/timer.h>
#include "appwindow.h"
#include "components/config.h"
#include "utils/cairo.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/system.h"

#define DEF_ICONNAME "data/images/docklight.home.ico"

DL_NS_BEGIN

bool Panel::m_mouse_inside;

Panel::Panel()
{
    m_app_updater.signal_update().connect(
        sigc::mem_fun(this, &Panel::on_appupdater_update));

    m_autohide.signal_update().connect(
        sigc::mem_fun(this, &Panel::on_autohide_update));

    // Set event masks
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK |
               Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK |
               Gdk::POINTER_MOTION_HINT_MASK | Gdk::FOCUS_CHANGE_MASK |
               Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK |
               Gdk::POINTER_MOTION_HINT_MASK | Gdk::POINTER_MOTION_MASK);

    Panel::m_mouse_inside = false;

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
        m_autohide.set_hide_delay(1.5);
        m_autohide.hide();
    }
}

Panel::~Panel()
{
    g_print("Free Panel\n");
}

void Panel::connect_draw_signal(bool connect)
{
    if (connect) {
        if (!m_connect_draw_signal_set) {
            m_sigc_draw = Glib::signal_timeout().connect(
                sigc::mem_fun(*this, &Panel::on_timeout_draw), 1000 / 8);

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

void Panel::on_autohide_update(int x, int y)
{
    m_offset_x = x;
    m_offset_y = y;

    Gtk::Widget::queue_draw();
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
bool m_enter = false;
bool Panel::on_motion_notify_event(GdkEventMotion* event)
{
    if (!m_enter) {
        int x = 0, y = 0, w = 0, h = 0;
        gdk_window_get_geometry(event->window, &x, &y, &w, &h);

        auto location = config::get_dock_location();
        if (location == dock_location_t::left) {
            if ((int)event->x < 10) {
                m_enter = true;
            }
        } else

            if (location == dock_location_t::bottom) {
            if ((int)event->y > h - 10) {
                m_enter = true;
            }
        }
        if (m_enter) {
            on_enter_notify_event(nullptr);
        }
    }

    //    g_print("Moution %d %d [%d]\n", (int)event->x, (int)event->y, 0);

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
    this->connect_draw_signal(true);

    if (!m_enter) {
        return true;
    }

    if (config::is_autohide() || config::is_intelihide()) {
        m_autohide.show();
    }

    // m_HomeMenu.hide();
    // m_ItemMenu.hide();

    return false;
}

bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    int x = 0, y = 0, w = 0, h = 0;
    gdk_window_get_geometry(crossing_event->window, &x, &y, &w, &h);

    // check for false positives
    if (config::get_dock_orientation() == Gtk::ORIENTATION_VERTICAL) {
        if ((int)crossing_event->y >= 0 && (int)crossing_event->y <= h) {
            if ((int)crossing_event->x == x) {
                return true;
            }
        }
    } else {
        if ((int)crossing_event->x >= 0 && (int)crossing_event->x <= w) {
            if ((int)crossing_event->y == h) {
                return true;
            }
        }
    }

    m_current_index = -1;
    on_timeout_draw();

    m_enter = false;
    Panel::m_mouse_inside = false;

    if (config::is_intelihide()) {
        m_autohide.intelihide();
    }
    if (config::is_autohide()) {
        m_autohide.hide();
    }

    this->connect_draw_signal(false);

    return true;
}

void Panel::on_active_window_changed(WnckScreen* screen,
                                     WnckWindow* previously_active_window,
                                     gpointer user_data)
{
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
    // cr->paint();
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
    // g_print("Draw %d\n", (int)items_count);

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
