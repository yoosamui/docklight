#include "components/panel.h"
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/menuitem.h>
#include "components/device.h"
#include "utils/launcher.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/system.h"
#include "utils/wnck.h"
DL_NS_BEGIN

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
    appinfo_t info;
    Panel_render::init();

    info.m_name = "desktop";
    info.m_title = _("Desktop");

    m_appupdater.m_dockitems.insert(m_appupdater.m_dockitems.begin(),
                                    shared_ptr<DockItem>(new DockItem(info)));

    auto icon_size = config::get_icon_size();
    load_home_icon(icon_size);

    m_appupdater.signal_update().connect(sigc::mem_fun(this, &Panel::on_appupdater_update));
    m_appupdater.init();

    m_autohide.init();

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

void Panel::connect_draw_signal(bool connect)
{
    if (connect) {
        if (!m_stm.m_connect_draw_signal_set) {
            m_stm.m_sigc_draw = Glib::signal_timeout().connect(
                sigc::mem_fun(static_cast<Panel*>(m_stm.m_this), &Panel::on_timeout_draw),
                1000 / 12);
            m_stm.m_connect_draw_signal_set = true;
        }
    } else {
        m_stm.m_sigc_draw.disconnect();
        m_stm.m_connect_draw_signal_set = false;
    }
}
void Panel::on_appupdater_update(window_action_t action, int index)
{
    // if window gets close put the index in a save position to avoid an segementation fault.
    if (action == window_action_t::CLOSE) {
        m_current_index = 0;
    }

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
    if (m_preview != nullptr) this->close_preview();

    m_offset_x = x;
    m_offset_y = y;

    Gtk::Widget::queue_draw();
}

int Panel::get_required_size()
{
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

        while (size > max_space) {
            m_stm.m_decrease_factor++;
            size = m_appupdater.get_required_size();
        }
    }

    return size + config::get_window_start_end_margin();
}
bool Panel::on_timeout_draw()
{
    if (!m_stm.m_dragdrop_begin && m_mouse_left_down && m_current_index > 0 &&
        m_drop_index == m_current_index && m_dragdrop_timer.elapsed() > 0.50) {
        m_stm.m_dragdrop_begin = true;

        this->close_preview();
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
    this->close_preview();

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
    this->on_appupdater_update(window_action_t::UPDATE, 0);
}

void Panel::on_home_menu_addseparator_event()
{
    Dock_menu::on_home_menu_addseparator_event();
    this->on_appupdater_update(window_action_t::UPDATE, 0);
}

void Panel::on_home_menu_quit_event()
{
    m_owner->close();
}

void Panel::on_item_menu_new_event()
{
    this->open_new();
}

void Panel::close_preview()
{
    if (m_preview != nullptr) {
        m_preview->close();

        m_preview.reset();
        m_preview = nullptr;

        m_preview_index = -1;

        if (!config::is_autohide_none()) {
            m_autohide.reset_timer();
            m_autohide.set_mouse_inside(false);
            if (config::is_autohide()) {
                m_autohide.hide();
            } else {
                m_autohide.intelihide();
            }
        }
    }
}
void Panel::show()
{
    m_autohide.set_mouse_inside(true);
    m_autohide.show();
}

bool Panel::on_motion_notify_event(GdkEventMotion* event)
{
    // TODO: optimize for speed
    m_current_index = this->get_index(event->x, event->y);
    /*int anchor_margin = config::get_anchor_margin();

    // detect anchor point
    if (!config::is_autohide_none() && !m_autohide.is_visible()) {
        auto const location = config::get_dock_location();
        int area = position_util::get_area();
        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            if (location == dock_location_t::top) {
                if (event->y < anchor_margin && event->y >= 0) {
                    this->show();
                }
            } else {
                if (event->y > area - anchor_margin) {
                    this->show();
                }
            }
        } else {
            if (location == dock_location_t::left) {
                if (event->x < anchor_margin && event->x >= 0) {
                    this->show();
                }
            } else {
                if (event->x > area - anchor_margin) {
                    this->show();
                }
            }
        }
    }*/

    // detect drag and drop
    if (m_stm.m_dragdrop_begin && m_current_index > 0) {
        if (!m_stm.m_mouse_inside) {
            this->stop_dragdrop();
            return true;
        }

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
void Panel::stop_dragdrop()
{
    m_dragdrop_timer.stop();
    m_dragdrop_timer.reset();

    m_stm.m_dragdrop_begin = false;

    // reset the swap item method and reset the index.
    m_drop_index = -1;
    m_appupdater.swap_item(0);
}

void Panel::build_windowlist_menu(GdkEventButton* event)
{
    // clang-format off

    auto const item = AppUpdater::m_dockitems[m_current_index];

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

bool Panel::on_button_release_event(GdkEventButton* event)
{
    // handle drop
    if (m_stm.m_dragdrop_begin && m_drop_index > 0) {
        this->stop_dragdrop();

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
                    this->close_preview();

                    // avoid panel hide
                    this->show();
                    this->open_new();
                } else {
                    if ((int)item->m_items.size() == 1) {
                        this->close_preview();

                        // avoid panel hide
                        this->show();
                        this->activate();

                    } else if (item->get_dock_item_type() == dock_item_type_t::launcher) {
                        // show preview if is compositite and not out of limits
                        //
                        if (m_preview != nullptr) {
                            if (m_preview_index == m_current_index) {
                                this->close_preview();

                                // avoid panel hide
                                this->show();
                                return true;
                            }
                        }

                        m_preview = unique_ptr<Panel_preview>(new Panel_preview());
                        m_preview_index = m_current_index;
                        m_preview->init(m_preview_index);
                        m_preview->signal_close().connect(
                            sigc::mem_fun(this, &Panel::close_preview));

                        bool can_show_preview = m_preview->show_preview();
                        if (!can_show_preview) {
                            build_windowlist_menu(event);
                        }
                    }
                }
            }
        } else if (event->button == 3 && m_mouse_right_down) {
            if (m_current_index != -1) {
                if (m_current_index == 0) {
                    // https://developer.gnome.org/gtkmm/stable/classGtk_1_1Menu.html#a84f2f1e74ba1a6316077a4dcee13ed87
                    //                https://developer.gnome.org/gtk3/stable/GtkMenu.html#GtkMenuPositionFunc
                    // https://developer.gnome.org/gtkmm/stable/classGtk_1_1Menu.html#a4b5f00c1684891c1362c5c6f8eee3fd1
                    //                    m_home_menu.set_monitor(0);
                    // m_home_menu.reposition();
                    //
                    if (!m_home_menu.get_attach_widget()) {
                        m_home_menu.attach_to_widget(*this);
                    }

                    m_home_menu.popup(sigc::mem_fun(*this, &Panel::on_home_menu_position), 1,
                                      event->time);
                    //

                    m_home_menu_addexpander_item.set_sensitive(!m_appupdater.is_exists_expander());
                    return true;
                }
                if (m_current_index > 0 &&
                    item->get_dock_item_type() == dock_item_type_t::launcher) {
                    m_item_menu_attach.set_active(item->is_attached());

                    /*// remove menu items
                    for (auto itemMenu : m_item_menu.get_children()) {
                        auto type = dynamic_cast<Gtk::ImageMenuItem*>(itemMenu);
                        if (type) {
                            m_item_menu.remove(*itemMenu);
                        }
                    }

                    for (size_t i = 0; i < item->m_items.size(); i++) {
                        auto const citem = item->m_items[(int)i];
                        Gtk::ImageMenuItem* menu_item =
                            Gtk::manage(new Gtk::ImageMenuItem(citem->get_windowname()));

                        const Glib::RefPtr<Gdk::Pixbuf>& pixbuf = item->get_image();
                        Gtk::Image* image = Gtk::manage(new Gtk::Image(pixbuf));
                        menu_item->set_image(*image);
                        menu_item->set_always_show_image(true);

                        menu_item->signal_activate().connect(sigc::bind<WnckWindow*>(
                            sigc::mem_fun(*this, &Panel::on_item_menu_windowlist_event),
                            citem->get_wnckwindow()));

                        m_item_menu.insert(*menu_item, 0);
                    }

                    m_item_menu.signal_show().connect(
                        sigc::mem_fun(*this, &Panel::on_menu_show_event));
                    m_item_menu.signal_hide().connect(
                        sigc::mem_fun(*this, &Panel::on_menu_hide_event));

                    m_item_menu.show_all_children(true);*/

                    // m_item_menu.popup_at_pointer(NULL);
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

    // reset the timer on enter
    if (!config::is_autohide_none()) {
        if (config::is_autohide() || config::is_intelihide()) {
            // resets the timer and set the mouse flag;
            //  m_autohide.set_mouse_inside(true);

            m_autohide.show();
        }
    }

    return false;
}

bool Panel::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    if (m_preview) {
        this->connect_draw_signal(false);
        return true;
    }

    this->stop_dragdrop();

    m_show_selector = false;

    // reset the timer on leave
    if (!config::is_autohide_none()) {
        if (config::is_autohide() || config::is_intelihide()) {
            // resets the timer and set the mouse flag;
            m_autohide.set_mouse_inside(false);
        }
    }

    auto const location = config::get_dock_location();
    int area = position_util::get_area();
    auto const workarea = position_util::get_workarea();
    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        if (location == dock_location_t::top) {
            if ((int)crossing_event->y <= 0) {
                return true;
            }
        } else {
            if ((int)crossing_event->y > area) {
                //    return true;
            }
        }
    } else {
        if (location == dock_location_t::left) {
            if ((int)crossing_event->x <= 0 && workarea.get_x() == 0) {
                return true;
            }
        } else {
            if ((int)crossing_event->x > area) {
                //  return true;
            }
        }
    }

    if (m_context_menu_open) {
        return true;
    }

    // remove current selection
    m_current_index = -1;
    this->draw();

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

    // avoid panel hide
    this->show();
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
                std::this_thread::sleep_for(std::chrono::milliseconds(600));

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

bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Panel_render::draw(cr);
    return true;
}

DL_NS_END

