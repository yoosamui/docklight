#include "components/panel.h"
#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/i18n.h>
#include <glibmm/main.h>
#include <glibmm/timer.h>
#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>
#include "appwindow.h"
#include "components/config.h"
#include "components/device.h"
#include "utils/cairo.h"
#include "utils/launcher.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/system.h"
#include "utils/wnck.h"

#define DEF_ICONNAME "data/images/docklight.home.ico"

DL_NS_BEGIN

bool Panel::m_mouse_inside;
int Panel::m_decrease_factor;

Panel::Panel()
{
    m_autohide.signal_update().connect(sigc::mem_fun(this, &Panel::on_autohide_update));

    // Set event masks
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK |
               Gdk::SMOOTH_SCROLL_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::FOCUS_CHANGE_MASK |
               Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK | Gdk::POINTER_MOTION_HINT_MASK |
               Gdk::POINTER_MOTION_MASK);

    Panel::m_mouse_inside = false;
    Panel::m_decrease_factor = 0;
}

void Panel::set_owner(Gtk::Window* window)
{
    m_owner = window;
}

void Panel::init()
{
    if (config::is_autohide()) {
        m_autohide.set_hide_delay(1.0);
        m_autohide.hide();
    }
    if (config::is_intelihide()) {
        m_autohide.set_hide_delay(0.5);
        m_autohide.intelihide();
    }

    // clang-format off

    // home menu
    m_home_menu.attach_to_widget(*this);
    m_home_menu.accelerate(*this);
    m_home_menu.signal_show().connect(sigc::mem_fun(*this, &Panel::on_menu_show_event));
    m_home_menu.signal_hide().connect(sigc::mem_fun(*this, &Panel::on_menu_hide_event));

    m_home_menu_quit_item.signal_activate().connect(sigc::mem_fun(*this, &Panel::on_home_menu_quit_event));
    m_home_menu_addseparator_item.signal_activate().connect(sigc::mem_fun(*this, &Panel::on_home_menu_addseparator_event));
    m_home_menu_showseparatorline_item.set_active(config::is_separator_line());
    m_home_menu_showseparatorline_item.signal_toggled().connect(sigc::mem_fun(*this, &Panel::on_home_menu_showline_event));

    // items menu
    m_item_menu.attach_to_widget(*this);
    m_item_menu.accelerate(*this);
    m_item_menu.signal_show().connect(sigc::mem_fun(*this, &Panel::on_menu_show_event));
    m_item_menu.signal_hide().connect(sigc::mem_fun(*this, &Panel::on_menu_hide_event));

    m_item_menu_new.signal_activate().connect(sigc::mem_fun(*this, &Panel::on_item_menu_new_event));

    m_item_menu_attach.set_active(false);
    m_item_menu_attach.signal_toggled().connect(sigc::mem_fun(*this, &Panel::on_item_menu_attach_event));

    // user separator
    m_separator_menu.attach_to_widget(*this);
    m_separator_menu.accelerate(*this);
    m_separator_menu.signal_show().connect(sigc::mem_fun(*this, &Panel::on_menu_show_event));
    m_separator_menu.signal_hide().connect(sigc::mem_fun(*this, &Panel::on_menu_hide_event));
    m_separator_menu_attach.set_active(false);
    m_separator_menu_attach.signal_toggled().connect(sigc::mem_fun(*this, &Panel::on_separator_menu_attach_event));

    // clang-format on
    string filename(system_util::get_current_path(DEF_ICONNAME));
    appinfo_t info;
    auto icon_size = config::get_icon_size();
    auto const homePixbuf = pixbuf_util::get_from_file(filename, icon_size, icon_size);

    info.m_name = "desktop";
    info.m_title = _("Desktop");

    m_appupdater.m_dockitems.insert(m_appupdater.m_dockitems.begin(),
                                    shared_ptr<DockItem>(new DockItem(info)));
    auto const new_item = m_appupdater.m_dockitems.back();
    new_item->set_image(homePixbuf);

    //    m_appupdater.init();

    m_appupdater.signal_update().connect(sigc::mem_fun(this, &Panel::on_appupdater_update));
    m_appupdater.init();
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
        // reset flags before disconect
        this->reset_flags();

        m_sigc_draw.disconnect();
        m_connect_draw_signal_set = false;
    }
}

void Panel::on_appupdater_update()
{
    if (m_autohide.is_visible() == false) {
        return;
    }

    AppWindow::update();
    this->draw();
}

void Panel::on_autohide_update(int x, int y)
{
    m_offset_x = x;
    m_offset_y = y;

    Gtk::Widget::queue_draw();
}
int Panel::get_required_size()
{
    m_decrease_factor = 0;
    Gdk::Rectangle workarea = device::monitor::get_current()->get_workarea();

    // the required full size
    int size = m_appupdater.get_required_size();
    int diff = 0;

    // ajust new items size
    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        if (size >= workarea.get_width()) {
            diff = (size - workarea.get_width()) + config::get_window_start_end_margin();
        }
    } else {
        if (size >= workarea.get_height()) {
            diff = (size - workarea.get_height()) + config::get_window_start_end_margin();
        }
    }

    if (diff > 0) {
        m_decrease_factor = diff / m_appupdater.m_dockitems.size();
        size = m_appupdater.get_required_size();

        // ajust the decrease factor if items are smaller than 0
        for (size_t i = 0; i < m_appupdater.m_dockitems.size(); i++) {
            auto const item = m_appupdater.m_dockitems[i];
            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                int item_width = item->get_width();

                if (item_width < 0) {
                    diff += abs(item_width);
                }
            } else {
                int item_height = item->get_height();

                if (item_height < 0) {
                    diff += abs(item_height);
                }
            }
        }

        m_decrease_factor = diff / m_appupdater.m_dockitems.size();
    }

    return size;
}

bool Panel::on_timeout_draw()
{
    if (m_current_index > 0 && !m_dragdrop_begin && m_mouse_left_down &&
        m_dragdrop_timer.elapsed() > 0.60) {
        m_dragdrop_begin = true;
        m_drop_index = m_current_index;

        // start blink  animation
        this->reset_flags();
    }

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
    int separator_margin = config::get_separator_margin();

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        for (auto item : m_appupdater.m_dockitems) {
            int width = item->get_width();
            if (width < 0) {
                idx++;
                continue;
            }

            if (mouse.get_x() >= x && mouse.get_x() <= x + width) {
                return idx;
            }

            x += width + separator_margin;
            idx++;
        }
    } else {
        for (auto item : m_appupdater.m_dockitems) {
            int height = item->get_height();
            if (height < 0) {
                idx++;
                continue;
            }

            if (mouse.get_y() >= y && mouse.get_y() <= y + height) {
                return idx;
            }

            y += height + separator_margin;
            idx++;
        }
    }

    return -1;
}

void Panel::draw()
{
    m_draw_required = true;
    on_timeout_draw();
}

void Panel::reset_flags()
{
    m_title_item_index = -1;
    m_inverted_index = -1;

    draw();
}

void Panel::on_menu_show_event()
{
    m_context_menu_open = true;
    this->connect_draw_signal(false);
}

void Panel::on_menu_hide_event()
{
    m_context_menu_open = false;

    if (config::is_intelihide() || config::is_autohide()) {
        m_autohide.reset_timer();

        if (config::is_intelihide()) {
            m_autohide.intelihide();
        } else if (config::is_autohide()) {
            m_autohide.hide();
        }
    }

    this->reset_flags();
}

void Panel::on_separator_menu_position(int& x, int& y, bool& push_in)
{
    this->get_center_position(x, y, m_separator_menu.get_width(), m_separator_menu.get_height());
}

void Panel::on_home_menu_position(int& x, int& y, bool& push_in)
{
    this->get_center_position(x, y, m_home_menu.get_width(), m_home_menu.get_height());
}

void Panel::on_item_menu_position(int& x, int& y, bool& push_in)
{
    this->get_center_position(x, y, m_item_menu.get_width(), m_item_menu.get_height());
}

void Panel::on_item_menu_windowlist_position(int& x, int& y, bool& push_in)
{
    this->get_center_position(x, y, m_item_menu_windowlist->get_width(),
                              m_item_menu_windowlist->get_height());
}

void Panel::on_item_menu_windowlist_event(WnckWindow* window)
{
    wnck_util::activate_window(window);
}

void Panel::on_home_menu_addseparator_event()
{
    appinfo_t info;
    string filename = system_util::get_current_path("/data/images/separator.png");
    // system_util::get_current_path("/data/images/docklight.logo.png");

    info.m_separator_length = 10;
    info.m_image =
        pixbuf_util::get_from_file(filename, info.m_separator_length, info.m_separator_length);
    info.m_name = "separator";
    info.m_title = _("separator");

    m_appupdater.m_dockitems.push_back(
        shared_ptr<DockItem>(new DockItem(info, dock_item_type_t::separator)));
    auto const new_item = m_appupdater.m_dockitems.back();
    new_item->set_attach(true);

    m_appupdater.save();
    this->on_appupdater_update();
}

void Panel::on_home_menu_quit_event()
{
    m_owner->close();
}
void Panel::on_item_menu_attach_event()
{
    bool attached = m_item_menu_attach.get_active();
    if (attached) {
        m_appupdater.attach_item(m_current_index);
    } else {
        m_appupdater.detach_item(m_current_index);
    }
}
void Panel::on_separator_menu_attach_event()
{
    bool attached = m_separator_menu_attach.get_active();
    if (attached) {
        m_appupdater.attach_item(m_current_index);
    } else {
        m_appupdater.detach_item(m_current_index);
    }
}

void Panel::on_home_menu_showline_event()
{
    bool showline = m_home_menu_showseparatorline_item.get_active();
    config::set_separator_line(showline);
}

void Panel::on_item_menu_new_event()
{
    this->open_new();
}

bool Panel::on_button_press_event(GdkEventButton* event)
{
    if ((event->type == GDK_BUTTON_PRESS)) {
        m_current_index = this->get_index(event->x, event->y);

        if (event->button == 1) {
            m_mouse_left_down = true;

            m_dragdrop_timer.reset();
            m_dragdrop_timer.start();

        } else if (event->button == 3) {
            m_mouse_right_down = true;
        }
    }
    return true;
}
bool Panel::on_button_release_event(GdkEventButton* event)
{
    if (m_autohide.is_visible() == false) {
        return true;
    }

    if (m_dragdrop_begin && m_drop_index != -1) {
        m_dragdrop_timer.stop();
        m_dragdrop_timer.reset();

        m_dragdrop_begin = false;

        // allways attach the drop item
        auto const item = AppUpdater::m_dockitems[m_drop_index];
        item->set_attach(true);

        // reset the swap item method.
        // and save the items with the new position.
        m_drop_index = -1;
        m_appupdater.swap_item(0);
        m_appupdater.save();

        return true;
    }

    if ((event->type == GDK_BUTTON_RELEASE) && m_current_index != -1) {
        auto const item = AppUpdater::m_dockitems[m_current_index];

        if (event->button == 1 && m_mouse_left_down) {
            m_mouse_left_down = false;

            if (m_current_index != -1) {
                if (item->m_items.size() == 0 &&
                    item->get_dock_item_type() == dock_item_type_t::launcher) {
                    this->open_new();
                } else {
                    if ((int)item->m_items.size() == 1) {
                        this->activate();
                    } else if (item->get_dock_item_type() == dock_item_type_t::launcher) {
                        // show preview if is compositite and not out of limits

                        // clang-format off

                        // build the window list menu
                        // delegate memory management to the container widget.
                        m_item_menu_windowlist = Gtk::manage(new Gtk::Menu());

                        for (size_t i = 0; i < item->m_items.size(); i++) {
                            auto const citem = item->m_items[(int)i];
                            Gtk::ImageMenuItem* menu_item = Gtk::manage(
                                    new Gtk::ImageMenuItem(citem->get_windowname()));

                            const Glib::RefPtr<Gdk::Pixbuf>& pixbuf = item->get_image();
                            Gtk::Image* image = Gtk::manage(new Gtk::Image(pixbuf));
                            menu_item->set_image(*image);
                            menu_item->set_always_show_image(true);

                            menu_item->signal_activate().connect(
                                    sigc::bind<WnckWindow*>(sigc::mem_fun(*this,
                                            &Panel::on_item_menu_windowlist_event),citem->get_wnckwindow()));

                            m_item_menu_windowlist->append(*menu_item);
                        }

                        m_item_menu_windowlist->show_all();
                        m_item_menu_windowlist->signal_show().connect(sigc::mem_fun(*this, &Panel::on_menu_show_event));
                        m_item_menu_windowlist->signal_hide().connect(sigc::mem_fun(*this, &Panel::on_menu_hide_event));
                        m_item_menu_windowlist->popup(sigc::mem_fun(*this,
                                    &Panel::on_item_menu_windowlist_position),1, event->time);

                        // clang-format on
                    }
                }
            }
        } else if (event->button == 3 && m_mouse_right_down) {
            if (m_current_index != -1) {
                if (m_current_index == 0) {
                    m_home_menu.popup(sigc::mem_fun(*this, &Panel::on_home_menu_position), 1,
                                      event->time);
                    return true;
                }
                if (m_current_index > 0 &&
                    item->get_dock_item_type() == dock_item_type_t::launcher) {
                    m_item_menu_attach.set_active(item->is_attached());
                    m_item_menu.popup(sigc::mem_fun(*this, &Panel::on_item_menu_position), 1,
                                      event->time);
                    return true;
                }
                if (m_current_index > 0 &&
                    item->get_dock_item_type() == dock_item_type_t::separator) {
                    m_separator_menu_attach.set_active(item->is_attached());
                    m_separator_menu.popup(sigc::mem_fun(*this, &Panel::on_separator_menu_position),
                                           1, event->time);
                    return true;
                }
            }
        }
    }
    return true;
}

bool Panel::on_scroll_event(GdkEventScroll* e)
{
    if (m_current_index < 1) return true;

    auto const item = AppUpdater::m_dockitems[m_current_index]->get_next();

    if (item == nullptr) return true;

    WnckWindow* window = item->get_wnckwindow();
    wnck_util::activate_window(window);

    return true;
}

bool Panel::on_motion_notify_event(GdkEventMotion* event)
{
    m_current_index = this->get_index(event->x, event->y);
    if (m_current_index > 0 && m_dragdrop_begin) {
        m_appupdater.swap_item(m_current_index);
        m_drop_index = m_current_index;
    }

    // force enter event
    on_enter_notify_event(nullptr);
    return false;
}

bool Panel::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_show_selector = true;
    Panel::m_mouse_inside = true;
    this->connect_draw_signal(true);

    if (config::is_autohide() || config::is_intelihide()) {
        m_autohide.reset_timer();
        m_autohide.set_mouse_inside(true);
    }

    // if (!m_enter_anchor) {
    // return true;
    //}

    if (config::is_autohide() || config::is_intelihide()) {
        m_autohide.show();
    }

    // m_HomeMenu.hide();
    // m_ItemMenu.hide();

    return false;
}

bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_show_selector = false;

    if (config::is_autohide() || config::is_intelihide()) {
        m_autohide.reset_timer();
        m_autohide.set_mouse_inside(false);
    }

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
            if ((int)crossing_event->y == h || (int)crossing_event->y == 0) {
                return true;
            }
        }
    }

    if (m_context_menu_open) {
        return true;
    }

    // remove current selection
    m_current_index = -1;
    this->draw();

    //  m_enter_anchor = false;
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

void Panel::open_new()
{
    if (m_current_index < 1) {
        return;
    }

    auto const item = AppUpdater::m_dockitems[m_current_index];
    m_show_selector = true;
    this->reset_flags();

    if (!launcher_util::launch(item->get_name(), item->get_desktop_filename())) {
        g_warning("Open new: App %s could not be found.", item->get_name().c_str());

        m_launcherwindow.init(item);
        m_launcherwindow.show_all();
    }
}

void Panel::activate()
{
    if (m_current_index < 1) {
        return;
    }

    auto const item = AppUpdater::m_dockitems[m_current_index];
    if (item == nullptr) return;
    if ((int)item->m_items.size() == 1) {
        auto const citem = item->m_items[0];
        WnckWindow* window = citem->get_wnckwindow();
        wnck_util::activate_window(window);
    }
}

void Panel::on_active_window_changed(WnckScreen* screen, WnckWindow* previously_active_window,
                                     gpointer user_data)
{
}

inline bool Panel::get_center_position(int& x, int& y, const int width, const int height)
{
    if (m_current_index < 0 || m_current_index > (int)AppUpdater::m_dockitems.size()) {
        return false;
    }

    auto const item = AppUpdater::m_dockitems[m_current_index];
    auto const rect = position_util::get_appwindow_geometry();
    auto const workarea = device::monitor::get_current()->get_workarea();
    auto const location = config::get_dock_location();
    auto const separator_margin = config::get_separator_margin();

    x = rect.get_x();
    y = rect.get_y();

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        x += config::get_window_start_end_margin() / 2;
        if (location == dock_location_t::bottom) {
            y = abs(y - height);
        } else {
            y += rect.get_height();
        }

        // clang-format off
        for (size_t i = 0; i < AppUpdater::m_dockitems.size(); i++) {
            auto const citem = AppUpdater::m_dockitems[i];
            auto const item_width = citem->get_width();
            if (citem == item) {
                x -= (width / 2) - (item_width / 2);

                // check left limit
                if (x < workarea.get_x()) {
                    x = workarea.get_x();
                    return true;
                }

                // check the limit on the right
                if (x + width > workarea.get_width()) {
                   // x = workarea.get_width() - width + workarea.get_x();
                    return true;
                }

                return true;
            }
            x += item_width + separator_margin;
        }
        // clang-format on

    } else {
        y += config::get_window_start_end_margin() / 2;
        if (location == dock_location_t::right) {
            x -= width;
        } else {
            x += rect.get_width();  // iockWindow::get_DockWindowWidth();
        }

        // clang-format off
        for (size_t i = 0; i < AppUpdater::m_dockitems.size(); i++) {
            auto const citem = AppUpdater::m_dockitems[i];
            auto const item_height = citem->get_height();
            if (citem == item) {
                y -= (height / 2) - (item_height / 2);

                // check left limit
                if (y < workarea.get_y()) {
                    y = workarea.get_y();
                    return true;
                }

                // check the limit on the right
                if (y + height > workarea.get_height()) {
                  //  y = workarea.get_height() - height + workarea.get_y();
                    return true;
                }

                return true;
            }
            y += item_height + separator_margin;
        }
        // clang-format on
    }

    return false;
}

inline void Panel::get_item_position(const dock_item_type_t item_typ, int& x, int& y, int& width,
                                     int& height)
{
    static int nextsize = 0;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        if (x == 0) {
            y = 4;

            if (m_decrease_factor > 0) {
                y = (config::get_dock_area() / 2) - (height / 2);
            }

            x = config::get_window_start_end_margin() / 2;
            nextsize = width;
            return;
        }

        x += nextsize + config::get_separator_margin();
        nextsize = width;
    } else {
        if (y == 0) {
            x = (config::get_dock_area() / 2) - (width / 2);
            y = config::get_window_start_end_margin() / 2;
            nextsize = height;
            return;
        }

        // if the item is a separator the wdth is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        /*if (dockType == DockItemType::Separator) {
            y += nextsize + Configuration::get_separatorMargin();
            height = nextsize = width;
            width = Configuration::get_CellWidth();
            return;
        }*/

        y += nextsize + config::get_separator_margin();
        nextsize = height;
    }
}

bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    this->draw_panel(cr);
    this->draw_items(cr);
    this->draw_title();
    return true;
}
void Panel::draw_panel(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Gdk::Rectangle rect = position_util::get_appwindow_geometry();

    // cr->set_source_rgba(1.8, 1.8, 1.8, 1.0);
    // cairo_util::rounded_rectangle(cr, rect.get_x(), rect.get_y(),
    //                              rect.get_width(), rect.get_height(), 3);

    // cr->stroke();
    rect.set_x(m_offset_x);
    rect.set_y(m_offset_y);
    cairo_util::rounded_rectangle(cr, rect.get_x(), rect.get_y(), rect.get_width(),
                                  rect.get_height(), 3);

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

    size_t items_count = m_appupdater.m_dockitems.size();
    // g_print("Draw %d\n", (int)items_count);

    // Draw all items with cairo
    for (size_t idx = 0; idx < items_count; idx++) {
        auto const item = m_appupdater.m_dockitems[idx];

        width = item->get_width();
        height = item->get_height();

        if (width < 1 || height < 1) {
            continue;
        }

        this->get_item_position(dock_item_type_t::launcher, x, y, width, height);

        item->set_x(x);
        item->set_y(y);

        // draw cell
        if (m_dragdrop_begin && (int)idx == m_drop_index) {
            cr->set_line_width(1.0);
            cr->set_source_rgba(0, 0, 1, 1);
            cairo_util::rounded_rectangle(cr, m_offset_x + x, m_offset_y + y, width, height, 3);
            cr->stroke();
        }

        // draw dots
        center = (width / 2);
        if (item->m_items.size() > 0) {
            cr->set_source_rgb(0.0, 0.0, 1.0);
            if (item->m_items.size() == 1) {
                cr->arc(m_offset_x + x + center, m_offset_y + y + area - 6 - m_decrease_factor, 1.5,
                        0, 2 * M_PI);
            } else if (item->m_items.size() > 1) {
                cr->arc(m_offset_x + x + center - 3, m_offset_y + y + area - 6 - m_decrease_factor,
                        1.5, 0, 2 * M_PI);
                cr->arc(m_offset_x + x + center + 3, m_offset_y + y + area - 6 - m_decrease_factor,
                        1.5, 0, 2 * M_PI);
            }
            cr->fill();
        }

        // separator
        if (config::is_separator_line() &&
            item->get_dock_item_type() == dock_item_type_t::separator) {
            cr->set_source_rgb(0, 0, 0);
            cr->set_line_width(0.4);

            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                int centerX = width / 2;
                cr->move_to(x + m_offset_x + centerX, y + m_offset_y);
                cr->line_to(x + m_offset_x + centerX, y + m_offset_y);
                cr->line_to(x + m_offset_x + centerX, y + m_offset_y + height);

            } else {
                int centerY = height / 2;
                cr->move_to(x + m_offset_x, y + m_offset_y + centerY);
                cr->line_to(x + m_offset_x, y + m_offset_y + centerY);
                cr->line_to(x + m_offset_x + width, y + m_offset_y + centerY);
            }

            cr->stroke();
        }

        // icon * selector
        if (auto image = item->get_image()) {
            // reload or scaled if needed
            if (image->get_width() != width || image->get_height() != height) {
                if (width > 0 && height > 0) {
                    auto const tmp_pixbuf = pixbuf_util::get_window_icon(
                        item->get_wnckwindow(), item->get_desktop_icon_name(), width);
                    if (!tmp_pixbuf) {
                        item->set_image(image->scale_simple(width, height, Gdk::INTERP_BILINEAR));
                    } else {
                        item->set_image(tmp_pixbuf);
                    }
                }
            }

            Gdk::Cairo::set_source_pixbuf(cr, item->get_image(), m_offset_x + x, m_offset_y + y);
            cr->paint();

            if (m_show_selector) {
                // draw selector
                if ((int)idx == m_current_index && !m_context_menu_open && m_mouse_inside &&
                    (int)idx != m_inverted_index) {
                    auto tmp = image->copy();
                    pixbuf_util::invert_pixels(tmp);
                    Gdk::Cairo::set_source_pixbuf(cr, tmp, m_offset_x + x, m_offset_y + y);

                    m_inverted_index = (int)m_current_index;
                    cr->paint();

                    // cr->set_source_rgba(1, 1, 1, 0.4);
                    // cairo_util::rounded_rectangle(cr, x, y, width, height, 0);
                    // cr->fill();
                }
            }
        }
    }
}
bool m_titlewindow_visible = false;
void Panel::draw_title()
{
    if (m_current_index < 0 || m_context_menu_open || !config::is_show_title() ||
        !m_connect_draw_signal_set || !m_autohide.is_visible()) {
        m_titlewindow.hide();
        m_titlewindow_visible = false;

        return;
    }

    if (!m_mouse_inside ||
        (m_current_index == m_title_item_index && m_title_timer.elapsed() > 4.0)) {
        m_title_timer.stop();
        m_title_timer.reset();
        m_titlewindow.hide();
        m_titlewindow_visible = false;

        return;
    }

    if (m_current_index != m_title_item_index) {
        m_title_timer.reset();
        m_title_timer.start();
        m_title_item_index = m_current_index;
        m_titlewindow.hide();
        m_titlewindow_visible = false;

        return;
    }

    // delay before show up
    if (m_title_timer.elapsed() < 0.8) {
        m_titlewindow.hide();
        m_titlewindow_visible = false;
        int y = 0, x = 0;
        this->get_center_position(x, y, 140, 26);
        m_titlewindow.move(x, y);
        return;
    }

    if (m_titlewindow_visible) {
        return;
    }

    m_titlewindow_visible = true;
    auto const item = AppUpdater::m_dockitems[m_current_index];
    int x = 0, y = 0;

    char title[60];
    int count = (int)item->m_items.size();
    if (count > 1) {
        sprintf(title, "%s (%d)", item->get_name().c_str(), count);
    } else {
        sprintf(title, "%s", item->get_name().c_str());
    }

    m_titlewindow.set_text(title);

    int height = m_titlewindow.get_height();
    int width = m_titlewindow.get_width();

    this->get_center_position(x, y, width, height);
    m_titlewindow.move(x, y);
}

DL_NS_END

