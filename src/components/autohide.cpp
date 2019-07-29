#include "autohide.h"
#include <gdkmm/rectangle.h>
#include <strings.h>
#include "components/config.h"
#include "utils/easing.h"
#include "utils/position.h"

//#include <glibmm/main.h>
DL_NS_BEGIN

static_members_t Autohide::m_stm;
WnckWindow* Autohide::m_active_window;

Autohide::Autohide()
{
    m_stm.m_this = static_cast<Autohide*>(this);
    m_stm.m_visible = false;

    Autohide::m_active_window = nullptr;

    WnckScreen* wnckscreen = wnck_screen_get_default();
    g_signal_connect(wnckscreen, "active_window_changed",
                     G_CALLBACK(Autohide::on_active_window_changed), nullptr);

    g_signal_connect(wnckscreen, "active_workspace_changed",
                     G_CALLBACK(Autohide::on_active_workspace_changed),
                     nullptr);
}

Autohide::~Autohide() {}

void Autohide::connect_signal_handler(bool connect)
{
    if (connect) {
        if (!m_stm.m_connect_autohide_signal_set) {
            m_stm.m_sigc_autohide = Glib::signal_timeout().connect(
                sigc::mem_fun((Autohide*)m_stm.m_this, &Autohide::animation),
                33.33);

            m_stm.m_connect_autohide_signal_set = true;
        }
    } else {
        m_stm.m_sigc_autohide.disconnect();
        m_stm.m_connect_autohide_signal_set = false;
    }

    if (m_stm.m_connect_autohide_signal_set)
        g_print("Connected\n");
    else
        g_print("disonnect\n");
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
    intelihide();

    if (previously_active_window != nullptr &&
        m_stm.m_geometry_change_id != 0) {
        g_signal_handler_disconnect(previously_active_window,
                                    m_stm.m_geometry_change_id);
    }

    m_stm.m_geometry_change_id =
        g_signal_connect(active_window, "geometry-changed",
                         G_CALLBACK(Autohide::on_geometry_change), nullptr);
}

void Autohide::on_active_workspace_changed(
    WnckScreen* screen, WnckWorkspace* previously_active_space,
    gpointer user_data)
{
    if (!config::is_intelihide() || m_active_window == nullptr) {
        return;
    }

    g_print("Windows count %d\n", Autohide::get_windows_count());

    if (Autohide::get_windows_count() == 0) {
        show();
        return;
    }

    intelihide();
}

void Autohide::on_geometry_change(WnckWindow* window, gpointer user_data)
{
    intelihide();
}

int Autohide::get_windows_count()
{
    int count = 0;
    WnckScreen* screen;
    GList* window_l;

    screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
         window_l = window_l->next) {
        //
        WnckWindow* window = WNCK_WINDOW(window_l->data);
        if (window == nullptr) continue;

        WnckWindowType wt = wnck_window_get_window_type(window);

        if (wt == WNCK_WINDOW_DESKTOP || wt == WNCK_WINDOW_DOCK ||
            wt == WNCK_WINDOW_TOOLBAR || wt == WNCK_WINDOW_MENU) {
            continue;
        }

        const char* instancename = wnck_window_get_class_instance_name(window);
        if (instancename != NULL &&
            strcasecmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
            continue;
        }

        count++;
    }

    return count;
}

void Autohide::intelihide()
{
    if (m_active_window == nullptr || config::is_intelihide() == false) {
        return;
    }

    int x = 0, y = 0, width = 0, height = 0;
    wnck_window_get_geometry(m_active_window, &x, &y, &width, &height);
    const Gdk::Rectangle rect_window(x, y, width, height);

    Gdk::Rectangle rect_dock = position_util::get_appwindow_geometry();

    connect_signal_handler(false);
    if (rect_dock.intersects(rect_window)) {
        // m_autohide_static_type.m_animation_timer.start();
        // if (m_visible) {
        // m_autohide_static_type.m_animation_state = DEF_HIDE;
        ////    Panel::connect_autohide_signal(true);
        //}
        //
        if (m_stm.m_visible) hide();
    } else {
        if (!m_stm.m_visible) show();
        // if (!m_visible) {
        // m_autohide_static_type.m_animation_state = DEF_SHOW;
        ////  Panel::connect_autohide_signal(true);
        //}
    }
}

void Autohide::set_hide_delay(float delay)
{
    m_animation_hide_delay = delay;
}

void Autohide::hide()
{
    m_stm.m_animation_state = DEF_AUTOHIDE_HIDE;
    m_stm.m_animation_timer.start();
    connect_signal_handler(true);
}

void Autohide::show()
{
    m_stm.m_animation_state = DEF_AUTOHIDE_SHOW;
    m_stm.m_animation_timer.start();
    connect_signal_handler(true);
}

bool Autohide::animation()
{
    if (m_stm.m_animation_state == DEF_AUTOHIDE_HIDE && !m_animation_running &&
        m_stm.m_visible &&
        abs(m_stm.m_animation_timer.elapsed()) > m_animation_hide_delay) {
        g_print("start hide\n");
        m_easing_duration = 5.0;
        m_animation_running = true;
    } else if (m_stm.m_animation_state == DEF_AUTOHIDE_SHOW &&
               !m_stm.m_visible && !m_animation_running) {
        g_print("start show\n");
        m_easing_duration = 4.0;
        m_animation_running = true;
    }

    if (m_animation_running) {
        auto endTime = m_initTime + m_easing_duration;
        float startPosition = 0.f;
        float endPosition = 0.f;

        auto location = config::get_dock_location();
        switch (location) {
            case dock_location_t::left:
            case dock_location_t::top: {
                if (m_stm.m_visible) {
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

        g_print("anim %d %d\n", m_stm.m_animation_state, (int)position);

        m_signal_update.emit(m_offset_x, m_offset_y);
        m_animation_time++;

        if ((int)position == (int)endPosition) {
            m_stm.m_animation_timer.stop();
            m_stm.m_animation_timer.reset();
            m_animation_running = false;
            m_animation_time = 0;

            m_stm.m_visible = (int)endPosition == 0;
            connect_signal_handler(false);
        }
    }

    return true;
}

DL_NS_END

