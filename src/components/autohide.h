#pragma once

#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <gdkmm/rectangle.h>
#include <glibmm/main.h>
#include <glibmm/timer.h>
#include <gmodule.h>
#include <gtkmm/enums.h>
#include <libwnck/libwnck.h>
#include <sigc++/sigc++.h>
#include "common.h"

DL_NS_BEGIN

#define DEF_AUTOHIDE_HIDE 0
#define DEF_AUTOHIDE_SHOW 1

typedef struct {
    sigc::connection m_sigc_autohide;
    bool m_connect_autohide_signal_set = false;
    int m_animation_state = DEF_AUTOHIDE_SHOW;
    gulong m_geometry_change_id = 0;
    gulong m_state_change_id = 0;
    Glib::Timer m_animation_timer;
    void* m_this;
    bool m_visible;
    bool m_mouse_inside = false;
    bool m_animation_running = false;
    Gdk::Rectangle m_last_window_geometry;
    bool m_fullscreen_window = false;
    WnckWindow* m_active_window = nullptr;
} static_members_t;

class Autohide
{
  public:
    Autohide();
    ~Autohide();

    static void hide();
    static void show();
    static void intelihide();

    void init();
    void set_mouse_inside(bool mouse_inside);
    void reset_timer();

    static bool is_visible();

    typedef sigc::signal<void, int, int> type_signal_update;
    type_signal_update signal_update();

  private:
    bool m_initialized = false;

    static static_members_t m_stm;

    type_signal_update m_signal_update;
    int m_animation_time = 0;
    int m_offset_x = 0;
    int m_offset_y = 0;
    float m_initTime = 0;
    float m_animation_hide_delay = 0.f;
    float m_easing_duration = 0.f;
    bool animation();

    static bool is_intersection_detected();
    static Gdk::Rectangle get_window_geometry(WnckWindow* window);
    static int get_windows_count(WnckWorkspace* workspace);
    static int get_windows_count();
    static void connect_signal_handler(bool connect);
    static void on_active_window_changed(WnckScreen* screen, WnckWindow* previously_active_window,
                                         gpointer user_data);

    static void on_geometry_changed(WnckWindow* window, gpointer user_data);
    static void on_state_changed(WnckWindow* window, WnckWindowState changed_mask,
                                 WnckWindowState new_state, gpointer user_data);

    static void on_active_workspace_changed_callback(WnckScreen* screen,
                                                     WnckWorkspace* previously_active_space,
                                                     gpointer user_data);
};

DL_NS_END

