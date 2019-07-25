#include "components/panel.h"
#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/main.h>
#include <glibmm/timer.h>
#include "appwindow.h"
#include "components/config.h"
#include "utils/cairo.h"
#include "utils/easing.h"
#include "utils/position.h"

DL_NS_BEGIN

bool Panel::m_mouse_inside;

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

    Panel::m_mouse_inside = false;

    if (config::is_autohidde()) {
        this->connect_autohidde_signal(true);
        m_animation_timer.start();
    }
}

Panel::~Panel() {}

void invert_pixels(Glib::RefPtr<Gdk::Pixbuf> image)
{
    gint x, y;
    // if (image:tabnewl
    // && image->get_colorspace() == Gdk::COLORSPACE_RGB &&
    // image->get_bits_per_sample() == 8){
    int w = image->get_width();
    int h = image->get_height();
    int channels = image->get_n_channels();
    gint rowstride = image->get_rowstride();
    gint pixel_offset;
    // for (i = 0; i < 4; i++) {
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            pixel_offset = y * rowstride + x * channels;
            guchar* pixel = &image->get_pixels()[pixel_offset];

            pixel[0] = 255 - pixel[0];
            pixel[1] = 255 - pixel[1];
            pixel[2] = 255 - pixel[2];
        }
    }

    //    DockPanel::update();
    //   std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }
}

// https://developer.gnome.org/gobject/unstable/gobject-Signals.html#g-signal-handler-disconnect
void Panel::connect_autohidde_signal(bool connect)
{
    if (connect && !m_connect_autohidde_signal_set) {
        m_sigc_autohidde = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &Panel::auto_hidde_timer), 33.33);

        m_connect_autohidde_signal_set = true;
        g_print("connect\n");
    } else {
        m_sigc_autohidde.disconnect();
        m_connect_autohidde_signal_set = false;

        g_print("disconnect\n");
    }
}

void Panel::connect_draw_signal(bool connect)
{
    if (connect && !m_connect_draw_signal_set) {
        m_sigc_draw = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &Panel::on_timeout_draw), 125.0);

        m_connect_draw_signal_set = true;

    } else {
        m_sigc_draw.disconnect();
        m_connect_draw_signal_set = false;
    }
}

void Panel::on_appupdater_update(bool a, int b)
{
    g_print("Update signal\n");
    AppWindow::update();

    m_draw_required = true;
    on_timeout_draw();
}

int Panel::get_required_size()
{
    int separators =
        config::get_separator_margin() * (m_app_updater.m_dockitems.size() - 1);
    //        config::get_separator_margin();
    //
    g_print("Size %d\n", (int)m_app_updater.m_dockitems.size());
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

    if (config::is_autohidde()) {
        this->connect_autohidde_signal(true);
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

    if (config::is_autohidde()) {
        this->connect_autohidde_signal(true);
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

float m_easing_duration = 6.0;
float m_initTime = 0;
int m_offset_x = 0;
int m_offset_y = 0;
bool Panel::auto_hidde_timer()
{
    // if (m_animation_state == 0) {
    //  return false;
    //}

    if (m_visible && !m_mouse_inside && !m_animation_running &&
        abs(m_animation_timer.elapsed()) > 1.5f) {
        g_print("hidde\n");
        m_easing_duration = 6.0;
        m_animation_running = true;
    }

    if (!m_visible && m_mouse_inside && !m_animation_running) {
        g_print("show\n");
        // this->set_gravity(m_gravity);
        //  this->set_gravity(Gdk::GRAVITY_NORTH_EAST);
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
                    ;
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

        /*if (m_visible) {
            startPosition = 0;  // config::get_dock_area();
            endPosition = config::get_dock_area();
        } else {
            startPosition = config::get_dock_area();
            endPosition = 0;
        }*/

        float position = easing_util::map_clamp(
            m_animation_time, m_initTime, endTime, startPosition, endPosition,
            &easing_util::linear::easeIn);

        // if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        ////   this->resize(this->get_width(), resize_pos);
        //} else {
        ////    this->resize(resize_pos, this->get_height());
        //}

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            m_offset_x = 0;
            m_offset_y = (int)position;
        } else {
            m_offset_x = (int)position;
            m_offset_y = 0;
        }

        g_print("anim %d\n", (int)position);

        Gtk::Widget::queue_draw();
        m_animation_time++;

        if ((int)position == (int)endPosition) {
            m_animation_timer.stop();
            m_animation_timer.reset();
            m_animation_running = false;
            m_animation_time = 0;

            // if (config::get_dock_orientation() ==
            // Gtk::ORIENTATION_HORIZONTAL) { m_visible = this->get_height() >
            // 1; g_print("END %d %d\n", (int)m_visible, this->get_height());
            //} else {
            // m_visible = (int)endPosition == 0;
            // g_print("END %d %d\n", (int)m_visible, this->get_width());
            //}

            m_visible = (int)endPosition == 0;  // config::get_dock_area();
            if (m_visible) {
                m_animation_timer.start();
            }
            connect_autohidde_signal(false);
            g_print("END %d %d\n", (int)m_visible, config::get_dock_area());
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
            invert_pixels(tmp);
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
