#include "components/panel.h"
#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/i18n.h>
#include <glibmm/main.h>
#include <glibmm/timer.h>
#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>
#include "appwindow.h"
#include "components/device.h"
#include "utils/cairo.h"
#include "utils/launcher.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/system.h"
#include "utils/wnck.h"

#define DEF_ICONNAME "data/images/docklight.home.ico"

DL_NS_BEGIN

panel_static_members_t Panel::m_stm;

Panel::Panel()
{
    m_stm.m_this = static_cast<Panel*>(this);
    m_autohide.signal_update().connect(sigc::mem_fun(this, &Panel::on_autohide_update));

    // Set event masks
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK |
               Gdk::SMOOTH_SCROLL_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::FOCUS_CHANGE_MASK |
               Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK | Gdk::POINTER_MOTION_HINT_MASK |
               Gdk::POINTER_MOTION_MASK);

    WnckScreen* wnckscreen = wnck_screen_get_default();
    g_signal_connect(wnckscreen, "active_window_changed",
                     G_CALLBACK(Panel::on_active_window_changed), nullptr);
}

void Panel::set_owner(Gtk::Window* window)
{
    m_owner = window;
}

void Panel::init()
{
    m_theme = config::get_theme();

    appinfo_t info;

    info.m_name = "desktop";
    info.m_title = _("Desktop");

    m_appupdater.m_dockitems.insert(m_appupdater.m_dockitems.begin(),
                                    shared_ptr<DockItem>(new DockItem(info)));
    //    auto const new_item = m_appupdater.m_dockitems.back();
    auto icon_size = config::get_icon_size();
    load_home_icon(icon_size);

    m_appupdater.signal_update().connect(sigc::mem_fun(this, &Panel::on_appupdater_update));
    m_appupdater.init();

    // if (config::is_autohide_none() == false) {
    m_autohide.set_hide_delay(0.5);
    m_autohide.init();
    //  }

    m_bck_thread = new thread(connect_async);
}

Panel::~Panel()
{
    // tell the background thread to terminate.
    m_stm.m_bck_thread_run = false;

    // Detach
    m_bck_thread->detach();

    // free memory
    delete m_bck_thread;

    // pointed dangling to ptr NULL
    m_bck_thread = nullptr;

    g_print("Free Panel\n");
}
void Panel::load_home_icon(int icon_size)
{
    string filename(system_util::get_current_path(DEF_ICONNAME));
    auto const home_pixbuf = pixbuf_util::get_from_file(filename, icon_size, icon_size);

    auto const home_item = AppUpdater::m_dockitems[0];
    home_item->set_image(home_pixbuf);
}
void Panel::connect_draw_signal(bool connect)
{
    if (connect) {
        if (!m_stm.m_connect_draw_signal_set) {
            m_stm.m_sigc_draw = Glib::signal_timeout().connect(
                sigc::mem_fun(static_cast<Panel*>(m_stm.m_this), &Panel::on_timeout_draw),
                1000 / 8);
            m_stm.m_connect_draw_signal_set = true;
        }
    } else {
        m_stm.m_sigc_draw.disconnect();
        m_stm.m_connect_draw_signal_set = false;
    }
}

void Panel::on_appupdater_update()
{
    if (!config::is_autohide_none()) {
        if (m_autohide.is_visible() == false) {
            return;
        }
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
    static int cache_size = 0;
    static int cache_factor = 0;
    static long cache_compare = 0;

    m_stm.m_decrease_factor = 0;

    auto const workarea = position_util::get_workarea();

    // the required full sizie
    int size = m_appupdater.get_required_size();
    int diff = 0;
    int max_space = 0;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        max_space = workarea.get_width() - config::get_window_start_end_margin();
    } else {
        max_space = workarea.get_height() - config::get_window_start_end_margin();
    }

    if (size > max_space) {
        diff = size - max_space;
    }

    if (diff > 0) {
        m_stm.m_decrease_factor = diff / m_appupdater.m_dockitems.size();
        size = m_appupdater.get_required_size();

        long compare = size * m_stm.m_decrease_factor;
        if (compare == cache_compare) {
            size = cache_size;
            m_stm.m_decrease_factor = cache_factor;

            goto realize;
        }

        cache_compare = compare;

        while (size > max_space) {
            m_stm.m_decrease_factor++;
            size = m_appupdater.get_required_size();
        }

        cache_size = size;
        cache_factor = m_stm.m_decrease_factor;
    }

realize:
    return size + config::get_window_start_end_margin();
}

bool Panel::on_timeout_draw()
{
    if (!m_stm.m_dragdrop_begin && m_mouse_left_down && m_current_index > 0 &&
        m_drop_index == m_current_index && m_dragdrop_timer.elapsed() > 0.50) {
        m_stm.m_dragdrop_begin = true;

        // start blink animation
        this->reset_flags();
    }

    if (m_stm.m_draw_required || m_stm.m_mouse_inside) {
        Gtk::Widget::queue_draw();
        m_stm.m_draw_required = false;
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
    m_stm.m_draw_required = true;
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

    if (!config::is_autohide_none()) {
        if (config::is_intelihide() || config::is_autohide()) {
            m_autohide.reset_timer();

            if (config::is_intelihide()) {
                m_autohide.intelihide();
            } else if (config::is_autohide()) {
                m_autohide.hide();
            }
        }
    }

    this->reset_flags();
}

void Panel::on_item_menu_windowlist_event(WnckWindow* window)
{
    wnck_util::activate_window(window);
}

void Panel::on_home_menu_addexpander_event()
{
    Dock_menu::on_home_menu_addexpander_event();
    this->on_appupdater_update();
}

void Panel::on_home_menu_addseparator_event()
{
    Dock_menu::on_home_menu_addseparator_event();
    this->on_appupdater_update();
}

void Panel::on_home_menu_quit_event()
{
    m_owner->close();
}

void Panel::on_item_menu_new_event()
{
    this->open_new();
}

bool Panel::on_motion_notify_event(GdkEventMotion* event)
{
    m_current_index = this->get_index(event->x, event->y);

    if (m_stm.m_dragdrop_begin && m_current_index > 0) {
        m_drop_index = m_current_index;

        m_appupdater.swap_item(m_drop_index);
    }

    return false;
}
bool Panel::on_button_press_event(GdkEventButton* event)
{
    if ((event->type == GDK_BUTTON_PRESS)) {
        m_current_index = this->get_index(event->x, event->y);

        m_mouse_click_event_time = gtk_get_current_event_time();

        if (event->button == 1) {
            m_mouse_left_down = true;
            if (m_current_index > 0) {
                m_drop_index = m_current_index;
                m_dragdrop_timer.reset();
                m_dragdrop_timer.start();
            }

        } else if (event->button == 3) {
            m_mouse_right_down = true;
        }
    }
    return true;
}
bool Panel::on_button_release_event(GdkEventButton* event)
{
    // handle drop
    if (m_stm.m_dragdrop_begin && m_drop_index > 0) {
        m_dragdrop_timer.stop();
        m_dragdrop_timer.reset();

        m_stm.m_dragdrop_begin = false;

        // reset the swap item method and reset the index.
        m_drop_index = -1;
        m_appupdater.swap_item(0);

        // allways attach all after drop
        m_appupdater.attach_all();

        return true;
    }

    if (!config::is_autohide_none()) {
        if (m_autohide.is_visible() == false) {
            return true;
        }
    }

    if ((event->type == GDK_BUTTON_RELEASE) && m_current_index != -1) {
        // Check for a valid Click.
        if ((gtk_get_current_event_time() - m_mouse_click_event_time) > 200) {
            return true;
        }
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

                    m_home_menu_addexpander_item.set_sensitive(!m_appupdater.is_exists_expander());
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
                    (item->get_dock_item_type() == dock_item_type_t::separator ||
                     item->get_dock_item_type() == dock_item_type_t::expander)) {
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

bool Panel::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_show_selector = true;
    m_stm.m_mouse_inside = true;
    this->connect_draw_signal(true);

    if (!config::is_autohide_none()) {
        if (config::is_autohide() || config::is_intelihide()) {
            m_autohide.reset_timer();
            m_autohide.set_mouse_inside(true);
        }

        if (config::is_autohide() || config::is_intelihide()) {
            m_autohide.show();
        }
    }
    // m_HomeMenu.hide();
    // m_ItemMenu.hide();

    return false;
}

bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_show_selector = false;
    if (!config::is_autohide_none()) {
        if (config::is_autohide() || config::is_intelihide()) {
            m_autohide.reset_timer();
            m_autohide.set_mouse_inside(false);
        }
    }

    auto const location = config::get_dock_location();
    int area = position_util::get_area();
    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        if (location == dock_location_t::top) {
            if ((int)crossing_event->y < 0) {
                return true;
            }
        } else {
            if ((int)crossing_event->y > area) {
                return true;
            }
        }
    } else {
        if (location == dock_location_t::left) {
            if ((int)crossing_event->x < 0) {
                return true;
            }
        } else {
            if ((int)crossing_event->x > area) {
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
    m_stm.m_mouse_inside = false;

    if (!config::is_autohide_none()) {
        if (config::is_intelihide()) {
            m_autohide.intelihide();
        } else if (config::is_autohide()) {
            m_autohide.hide();
        }
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
        if (window != nullptr) {
            wnck_util::activate_window(window);
        }
    }
}

void Panel::connect_async()
{
    while (m_stm.m_bck_thread_run) {
        if (m_stm.m_bck_thread_connect) {
            m_stm.m_bck_thread_connect = false;

            if (m_stm.m_connect_draw_signal_set == false) {
                m_stm.m_draw_required = true;
                connect_draw_signal(true);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));

                m_stm.m_draw_required = false;
                connect_draw_signal(false);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 9));
    }
    connect_draw_signal(false);
}
void Panel::on_active_window_changed(WnckScreen* screen, WnckWindow* previously_active_window,
                                     gpointer user_data)
{
    WnckWindow* window = wnck_screen_get_active_window(screen);
    if (window == nullptr) {
        return;
    }

    set_active_window_indexp(window);
}

void Panel::set_active_window_indexp(WnckWindow* window)
{
    m_stm.m_active_window_index = -1;

    if (m_stm.m_dragdrop_begin == false) {
        bool found = false;
        for (size_t i = 1; i < AppUpdater::m_dockitems.size(); i++) {
            auto const item = AppUpdater::m_dockitems[i];
            if (!item) continue;

            for (size_t idx = 0; idx < item->m_items.size(); idx++) {
                auto const citem = item->m_items[idx];
                if (!citem) continue;

                if (citem->get_wnckwindow() == window) {
                    m_stm.m_active_window_index = i;
                    found = true;
                    break;
                }
            }

            if (found) {
                break;
            }
        }
    }

    m_stm.m_bck_thread_connect = true;
}

inline bool Panel::get_center_positionX(int& x, int& y, const int width, const int height)
{
    if (m_current_index < 0 || m_current_index > (int)AppUpdater::m_dockitems.size()) {
        return false;
    }

    auto const item = AppUpdater::m_dockitems[m_current_index];
    auto const rect = position_util::get_appwindow_geometry();
    auto const workarea = position_util::get_workarea();
    auto const location = config::get_dock_location();

    x = rect.get_x();
    y = rect.get_y();

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        x += config::get_window_start_end_margin() / 2;
        if (location == dock_location_t::bottom) {
            y = abs(y - height);
        } else {
            y += rect.get_height();
        }

        for (size_t i = 0; i < AppUpdater::m_dockitems.size(); i++) {
            auto const citem = AppUpdater::m_dockitems[i];
            auto const item_width = citem->get_width();
            if (citem == item) {
                int center = (item_width / 2) - (width / 2);
                x = (rect.get_x() + citem->get_x()) + center;

                if (x + width > workarea.get_width()) {
                    x = workarea.get_width() - width;
                }

                if (x < workarea.get_x()) {
                    x = workarea.get_x();
                }

                return true;
            }
        }

    } else {
        y += config::get_window_start_end_margin() / 2;
        if (location == dock_location_t::right) {
            x -= width;
        } else {
            x += rect.get_width();  // iockWindow::get_DockWindowWidth();
        }

        for (size_t i = 0; i < AppUpdater::m_dockitems.size(); i++) {
            auto const citem = AppUpdater::m_dockitems[i];
            auto const item_height = citem->get_height();

            if (citem == item) {
                int center = (item_height / 2) - (height / 2);
                y = (rect.get_y() + citem->get_y()) + center;

                if (y + height > workarea.get_height()) {
                    y = workarea.get_height() - height;
                }

                if (y < workarea.get_y()) {
                    y = workarea.get_y();
                }

                return true;
            }
        }
    }

    return false;
}

inline void Panel::get_item_position(const dock_item_type_t item_typ, int& x, int& y, int& width,
                                     int& height)
{
    static int nextsize = 0;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        if (x == 0) {
            y = 0;

            x = config::get_window_start_end_margin() / 2;
            nextsize = width;
            return;
        }

        x += nextsize + config::get_separator_margin();
        nextsize = width;
    } else {
        if (y == 0) {
            x = 0;
            y = config::get_window_start_end_margin() / 2;
            nextsize = height;
            return;
        }

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
    rect.set_x(m_offset_x);
    rect.set_y(m_offset_y);

    // clang-format off
    if (m_theme.Panel().Fill().Color::alpha != 0.0) {

        cairo_util::rounded_rectangle(cr, rect.get_x(),
                                          rect.get_y(),
                                          rect.get_width(),
                                          rect.get_height(),
                                          m_theme.Panel().Ratio());


        cr->set_source_rgba(m_theme.Panel().Fill().Color::red,
                            m_theme.Panel().Fill().Color::green,
                            m_theme.Panel().Fill().Color::blue,
                            m_theme.Panel().Fill().Color::alpha);

      cr->fill();
    // cr->paint();
    }

    if (m_theme.Panel().Stroke().Color::alpha != 0.0) {

        cairo_util::rounded_rectangle(cr, rect.get_x(),
                                          rect.get_y(),
                                          rect.get_width(),
                                          rect.get_height(),
                                          m_theme.Panel().Ratio());

        cr->set_source_rgba(m_theme.Panel().Stroke().Color::red,
                            m_theme.Panel().Stroke().Color::green,
                            m_theme.Panel().Stroke().Color::blue,
                            m_theme.Panel().Stroke().Color::alpha);


        cr->stroke();
    }
    // clang-format on
}
void Panel::draw_items(const Cairo::RefPtr<Cairo::Context>& cr)
{
    int y = 0;
    int x = 0;
    int width = 0;
    int height = 0;
    int initial_icon_size = config::get_icon_size();
    int icon_size = initial_icon_size;
    Gtk::Orientation orientation = config::get_dock_orientation();
    int area = position_util::get_area();
    size_t items_count = m_appupdater.m_dockitems.size();

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

        if (orientation == Gtk::ORIENTATION_HORIZONTAL) {
            y = m_offset_y;
        } else {
            x = m_offset_x;
        }

        // separator
        this->draw_separator(cr, item, x, y, width, height, orientation);

        // cell
        this->draw_cell(cr, item, x, y, width, height, area, orientation);

        // drag indicator
        this->draw_drag_indicator(cr, item, x, y, idx, width, height, orientation);

        // draw active window selector
        this->draw_active_window_indicator(cr, x, y, idx, width, height);

        // icon + selector
        auto const icon = item->get_image();
        if (icon) {
            icon_size = initial_icon_size;
            if (m_stm.m_decrease_factor > 0) {
                icon_size -= m_stm.m_decrease_factor;
            }

            this->draw_icon(cr, item, icon, icon_size, area, idx, x, y);
        }

        // draw indicator
        this->draw_indicator(cr, item, area, x, y, width, height);
    }
}

inline void Panel::draw_cell(const Cairo::RefPtr<Cairo::Context>& cr,
                             const shared_ptr<DockItem>& item, int x, int y, int width, int height,
                             int area, Gtk::Orientation orientation) const
{
    if (item->get_dock_item_type() == dock_item_type_t::launcher) {
        if (m_theme.PanelCell().Fill().Color::alpha > 0.0) {
            cr->set_source_rgba(
                m_theme.PanelCell().Fill().Color::red, m_theme.PanelCell().Fill().Color::green,
                m_theme.PanelCell().Fill().Color::blue, m_theme.PanelCell().Fill().Color::alpha);

            cairo_util::rounded_rectangle(cr, x, y + 1, width, height - 2,
                                          m_theme.PanelCell().Ratio());
            cr->fill();
        }

        if (m_theme.PanelCell().Stroke().Color::alpha > 0.0) {
            cr->set_source_rgba(m_theme.PanelCell().Stroke().Color::red,
                                m_theme.PanelCell().Stroke().Color::green,
                                m_theme.PanelCell().Stroke().Color::blue,
                                m_theme.PanelCell().Stroke().Color::alpha);

            cr->set_line_width(m_theme.PanelCell().LineWidth());

            cairo_util::rounded_rectangle(cr, x, y + 1, width, height - 2,
                                          m_theme.PanelCell().Ratio());
            cr->stroke();
        }
    }
}

inline void Panel::draw_indicator(const Cairo::RefPtr<Cairo::Context>& cr,
                                  const shared_ptr<DockItem>& item, int area, int x, int y,
                                  int width, int height) const
{
    if (item->get_dock_item_type() != dock_item_type_t::launcher) {
        return;
    }

    if (item->m_items.size() < 1) {
        return;
    }

    auto const indicator_type = config::get_indicator_type();
    int center = (width / 2);

    if (indicator_type == dock_indicator_type_t::dots &&
        m_theme.PanelIndicator().Fill().Color::alpha > 0.0) {
        if (item->m_items.size() == 1) {
            cr->arc(x + center, y + area - 4, 1.8, 0, 2 * M_PI);
        } else if (item->m_items.size() > 1) {
            cr->arc(x + center - 3, y + area - 4, 1.8, 0, 2 * M_PI);
            cr->arc(x + center + 3, y + area - 4, 1.8, 0, 2 * M_PI);
        }

        cr->set_source_rgba(m_theme.PanelIndicator().Fill().Color::red,
                            m_theme.PanelIndicator().Fill().Color::green,
                            m_theme.PanelIndicator().Fill().Color::blue,
                            m_theme.PanelIndicator().Fill().Color::alpha);
        cr->fill();

    } else if (indicator_type == dock_indicator_type_t::lines &&
               m_theme.PanelIndicator().Stroke().Color::alpha > 0.0) {
        int marginY = area - 4;
        if (item->m_items.size() > 0) {
            if (item->m_items.size() == 1) {
                cr->move_to(x + 0, y + marginY);
                cr->line_to(x + 0, y + marginY);
                cr->line_to(x + width - 0, y + marginY);

            } else if (item->m_items.size() > 1) {
                cr->move_to(x + 0, y + marginY);
                cr->line_to(x + 0, y + marginY);
                cr->line_to(x + center - 3, y + marginY);

                cr->move_to(x + center + 3, y + marginY);
                cr->line_to(x + center + 3, y + marginY);
                cr->line_to(x + width - 0, y + marginY);
            }
        }

        cr->set_source_rgba(m_theme.PanelIndicator().Stroke().Color::red,
                            m_theme.PanelIndicator().Stroke().Color::green,
                            m_theme.PanelIndicator().Stroke().Color::blue,
                            m_theme.PanelIndicator().Stroke().Color::alpha);

        cr->set_line_width(2.0);
        cr->stroke();
    }
}

inline void Panel::draw_active_window_indicator(const Cairo::RefPtr<Cairo::Context>& cr, int x,
                                                int y, int idx, int width, int height) const
{
    if (!m_stm.m_dragdrop_begin && idx > 0 && m_stm.m_active_window_index == (int)idx) {
        cairo_util::rounded_rectangle(cr, x, y, width, height, 0);
        cr->set_source_rgba(1, 1, 1, 0.2);
        cr->fill();
    }
}

inline void Panel::draw_drag_indicator(const Cairo::RefPtr<Cairo::Context>& cr,
                                       const shared_ptr<DockItem>& item, int x, int y, int idx,
                                       int width, int height, Gtk::Orientation orientation) const
{
    if (m_stm.m_dragdrop_begin && idx == m_drop_index) {
        if (m_theme.PanelDrag().Fill().Color::alpha > 0.0) {
            cr->set_source_rgba(
                m_theme.PanelDrag().Fill().Color::red, m_theme.PanelDrag().Fill().Color::green,
                m_theme.PanelDrag().Fill().Color::blue, m_theme.PanelDrag().Fill().Color::alpha);

            cairo_util::rounded_rectangle(cr, x, y, width, height, m_theme.PanelDrag().Ratio());
            cr->fill();
        }

        if (m_theme.PanelDrag().Stroke().Color::alpha > 0.0) {
            cr->set_source_rgba(m_theme.PanelDrag().Stroke().Color::red,
                                m_theme.PanelDrag().Stroke().Color::green,
                                m_theme.PanelDrag().Stroke().Color::blue,
                                m_theme.PanelDrag().Stroke().Color::alpha);

            cr->set_line_width(m_theme.PanelDrag().LineWidth());
            cairo_util::rounded_rectangle(cr, x, y, width, height, m_theme.PanelDrag().Ratio());
            cr->stroke();
        }
    }
}

inline void Panel::draw_separator(const Cairo::RefPtr<Cairo::Context>& cr,
                                  const shared_ptr<DockItem>& item, const int x, const int y,
                                  const int width, const int height,
                                  const Gtk::Orientation orientation)

{
    if (config::is_separator_line() && m_theme.PanelSeparator().Stroke().Color::alpha > 0.0 &&
        item->get_dock_item_type() == dock_item_type_t::separator) {
        cr->set_source_rgba(m_theme.PanelSeparator().Stroke().Color::red,
                            m_theme.PanelSeparator().Stroke().Color::green,
                            m_theme.PanelSeparator().Stroke().Color::blue,
                            m_theme.PanelSeparator().Stroke().Color::alpha);

        cr->set_line_width(m_theme.PanelSeparator().LineWidth());

        if (orientation == Gtk::ORIENTATION_HORIZONTAL) {
            int center = item->get_width() / 2;
            cr->move_to(x + center, y + 6);
            cr->line_to(x + center, y + 6);
            cr->line_to(x + center, y + height - 6);
        } else {
            int center = item->get_height() / 2;
            cr->move_to(x + 6, y + center);
            cr->line_to(x + 6, y + center);
            cr->line_to(x + width - 6, y + center);
        }

        cr->stroke();
    }
}

inline void Panel::draw_icon(const Cairo::RefPtr<Cairo::Context>& cr,
                             const shared_ptr<DockItem>& item,
                             const Glib::RefPtr<Gdk::Pixbuf>& icon, int icon_size, int area,
                             int idx, int x, int y)
{
    if (icon && item->get_dock_item_type() == dock_item_type_t::launcher) {
        // reload or scaled if needed
        if (icon->get_height() != icon_size || icon->get_width() != icon_size) {
            // load home icon
            load_home_icon(icon_size);

            // Get a fresh new icon
            auto const tmp_pixbuf = pixbuf_util::get_window_icon(
                item->get_wnckwindow(), item->get_desktop_icon_name(), icon_size);

            if (!tmp_pixbuf) {
                item->set_image(icon->scale_simple(icon_size, icon_size, Gdk::INTERP_BILINEAR));
            } else {
                item->set_image(tmp_pixbuf);
            }
        }

        int center_pos_x = (area / 2) - (icon_size / 2);
        Gdk::Cairo::set_source_pixbuf(cr, item->get_image(), x + center_pos_x, y + 4);
        cr->paint();

        if (m_show_selector) {
            //  draw selector
            if ((int)idx == m_current_index && !m_context_menu_open && m_stm.m_mouse_inside &&
                (int)idx != m_inverted_index) {
                // create copy and invert image colors
                auto tmp = item->get_image()->copy();
                pixbuf_util::invert_pixels(tmp);
                Gdk::Cairo::set_source_pixbuf(cr, tmp, x + center_pos_x, y + 4);

                m_inverted_index = (int)m_current_index;
                cr->paint();

                // normal selector cell
                // cr->set_source_rgba(1, 1, 1, 0.2);
                // cairo_util::rounded_rectangle(cr, x, y, width, height, 0);
                // cr->fill();
            }
        }
    }
}

void Panel::draw_title()
{
    if (m_current_index < 0 || m_context_menu_open || !config::is_show_title() ||
        !m_stm.m_connect_draw_signal_set || !m_autohide.is_visible()) {
        m_titlewindow.hide();
        m_titlewindow_visible = false;

        return;
    }

    if (!m_stm.m_mouse_inside ||
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
        position_util::get_center_position(m_current_index, x, y, 140, 26);
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
        sprintf(title, "%s (%d)", item->get_title().c_str(), count);
    } else {
        sprintf(title, "%s", item->get_title().c_str());
    }

    m_titlewindow.set_text(title);

    int height = m_titlewindow.get_height();
    int width = m_titlewindow.get_width();

    position_util::get_center_position(m_current_index, x, y, width, height);
    m_titlewindow.move(x, y);
}

DL_NS_END

