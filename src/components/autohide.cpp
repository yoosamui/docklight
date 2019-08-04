#include "autohide.h"
#include <strings.h>
#include "components/config.h"
#include "components/device.h"
#include "utils/easing.h"
#include "utils/position.h"

DL_NS_BEGIN

#define DEF_AUTOHIDE_EASING_DURATION 4.0

static_members_t Autohide::m_stm;
WnckWindow* Autohide::m_active_window;

Autohide::Autohide()
{
    m_stm.m_this = static_cast<Autohide*>(this);
    m_stm.m_visible = true;

    Autohide::m_active_window = nullptr;

    WnckScreen* wnckscreen = wnck_screen_get_default();

    g_signal_connect(wnckscreen, "active_window_changed",
                     G_CALLBACK(Autohide::on_active_window_changed), nullptr);
}

Autohide::~Autohide()
{
    g_print("Free autohide\n");
}

void Autohide::connect_signal_handler(bool connect)
{
    if (connect) {
        if (!m_stm.m_connect_autohide_signal_set) {
            m_stm.m_sigc_autohide = Glib::signal_timeout().connect(
                sigc::mem_fun((Autohide*)m_stm.m_this, &Autohide::animation),
                1000 / 30);

            m_stm.m_connect_autohide_signal_set = true;
        }
    } else {
        m_stm.m_sigc_autohide.disconnect();
        m_stm.m_connect_autohide_signal_set = false;
    }
}

Autohide::type_signal_update Autohide::signal_update()
{
    return m_signal_update;
}

void Autohide::on_active_window_changed(WnckScreen* screen,
                                        WnckWindow* previously_active_window,
                                        gpointer user_data)
{
    if (!config::is_intelihide()) {
        return;
    }

    WnckWindow* active_window = wnck_screen_get_active_window(screen);
    if (!active_window) {
        return;
    }

    m_active_window = active_window;

    WnckWindowType wt = wnck_window_get_window_type(m_active_window);
    if (wt == WNCK_WINDOW_DESKTOP || m_stm.m_mouse_inside) {
        show();
    } else {
        intelihide();
    }

    if (previously_active_window != nullptr) {
        if (m_stm.m_geometry_change_id != 0) {
            g_signal_handler_disconnect(previously_active_window,
                                        m_stm.m_geometry_change_id);
        }

        if (m_stm.m_state_change_id != 0) {
            g_signal_handler_disconnect(previously_active_window,
                                        m_stm.m_state_change_id);
        }
    }

    // clang-format off
    m_stm.m_geometry_change_id = g_signal_connect_after(
        active_window, "geometry-changed",
        G_CALLBACK(Autohide::on_geometry_changed), nullptr);

    m_stm.m_state_change_id =  g_signal_connect_after(
        active_window, "state-changed",
        G_CALLBACK(Autohide::on_state_changed), nullptr);

    // clang-format on
}

void Autohide::on_state_changed(WnckWindow* window,
                                WnckWindowState changed_mask,
                                WnckWindowState new_state, gpointer user_data)
{
    if ((changed_mask & WNCK_WINDOW_STATE_MINIMIZED) == 0) {
        return;
    }

    intelihide();
}

void Autohide::on_geometry_changed(WnckWindow* window, gpointer user_data)
{
    if (m_active_window == nullptr || config::is_intelihide() == false) {
        return;
    }

    auto rect_window = Autohide::get_window_geometry(window);
    if (rect_window == m_stm.m_last_window_geometry) {
        return;
    }

    m_stm.m_last_window_geometry == rect_window;

    WnckWindowType wt = wnck_window_get_window_type(m_active_window);
    if (wt == WNCK_WINDOW_DESKTOP) {
        return;
    }

    intelihide();
}

int Autohide::get_windows_count()
{
    if (m_active_window == nullptr) {
        return -1;
    }

    WnckScreen* screen = wnck_window_get_screen(m_active_window);
    WnckWorkspace* workspace = wnck_screen_get_active_workspace(screen);

    if (workspace == nullptr) {
        return -1;
    }

    return get_windows_count(workspace);
}

int Autohide::get_windows_count(WnckWorkspace* workspace)
{
    int count = 0;
    WnckScreen* screen;
    GList* window_l;

    screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
         window_l = window_l->next) {
        WnckWindow* window = WNCK_WINDOW(window_l->data);
        if (window == nullptr ||
            !wnck_window_is_on_workspace(window, workspace) ||
            wnck_window_is_minimized(window)) {
            continue;
        }

        WnckWindowType wt = wnck_window_get_window_type(window);

        if (wt == WNCK_WINDOW_DESKTOP || wt == WNCK_WINDOW_DOCK ||
            wt == WNCK_WINDOW_TOOLBAR || wt == WNCK_WINDOW_MENU ||
            wt == WNCK_WINDOW_SPLASHSCREEN) {
            continue;
        }

        const char* instancename = wnck_window_get_class_instance_name(window);
        if (instancename != nullptr &&
            strcasecmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
            continue;
        }

        count++;
    }

    return count;
}

Gdk::Rectangle Autohide::get_window_geometry(WnckWindow* window)
{
    Gdk::Rectangle result(0, 0, 0, 0);
    if (window == nullptr) {
        return result;
    }

    int x = 0, y = 0, width = 0, height = 0;
    wnck_window_get_geometry(window, &x, &y, &width, &height);
    result.set_x(x);
    result.set_y(y);
    result.set_width(width);
    result.set_height(height);

    return result;
}

bool Autohide::is_intersection_detected()
{
    if (m_active_window == nullptr) {
        return false;
    }

    auto rect_dock = position_util::get_appwindow_geometry();
    auto rect_window = Autohide::get_window_geometry(m_active_window);

    return rect_window.intersects(rect_dock);
}

void Autohide::intelihide()
{
    if (m_active_window == nullptr || config::is_intelihide() == false) {
        return;
    }

    if (Autohide::is_intersection_detected()) {
        if (Autohide::get_windows_count() == 0) {
            return;
        }

        hide();

    } else {
        show();
    }
}

void Autohide::set_hide_delay(float delay)
{
    m_animation_hide_delay = delay;
}

void Autohide::hide()
{
    if (m_stm.m_visible) {
        m_stm.m_animation_state = DEF_AUTOHIDE_HIDE;
        connect_signal_handler(true);
    }
}

void Autohide::show()
{
    if (!m_stm.m_visible) {
        if (m_active_window && wnck_window_is_fullscreen(m_active_window)) {
            return;
        }

        m_stm.m_animation_state = DEF_AUTOHIDE_SHOW;
        connect_signal_handler(true);
    }
}

void Autohide::set_mouse_inside(bool mouse_inside)
{
    m_stm.m_mouse_inside = mouse_inside;
}

void Autohide::reset_timer()
{
    // if (!config::is_autohide()) {
    // return;
    //}

    m_stm.m_animation_state = DEF_AUTOHIDE_SHOW;
    m_stm.m_animation_timer.stop();
    m_stm.m_animation_timer.reset();

    m_stm.m_animation_timer.start();
}

bool Autohide::animation()
{
    if (m_stm.m_animation_state == DEF_AUTOHIDE_HIDE &&
        !m_stm.m_animation_running &&
        abs(m_stm.m_animation_timer.elapsed()) > m_animation_hide_delay) {
        m_easing_duration = DEF_AUTOHIDE_EASING_DURATION;
        m_stm.m_animation_running = true;

    } else if (m_stm.m_animation_state == DEF_AUTOHIDE_SHOW &&
               !m_stm.m_visible && !m_stm.m_animation_running) {
        m_easing_duration = DEF_AUTOHIDE_EASING_DURATION;
        m_stm.m_animation_running = true;
    }

    if (m_stm.m_animation_running) {
        auto endTime = m_initTime + m_easing_duration;
        float startPosition = 0.f;
        float endPosition = 0.f;

        auto location = config::get_dock_location();
        switch (location) {
            case dock_location_t::left:
            case dock_location_t::top: {
                if (m_stm.m_animation_state == DEF_AUTOHIDE_HIDE) {
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
                if (m_stm.m_visible) {
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

        m_signal_update.emit(m_offset_x, m_offset_y);
        m_animation_time++;

        if ((int)position == (int)endPosition) {
            this->reset_timer();
            m_stm.m_animation_running = false;
            m_animation_time = 0;

            m_stm.m_visible = (int)endPosition == 0;
            connect_signal_handler(false);

            if (m_stm.m_visible) m_stm.m_animation_timer.start();
        }
    }

    return true;
}

DL_NS_END

