
#pragma once

#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>
#include "common.h"
#include "components/appupdater.h"

#include <glibmm/timer.h>
DL_NS_BEGIN

#define DEF_HIDE 0
#define DEF_SHOW 1

typedef struct {
    sigc::connection m_sigc_autohide;
    bool m_connect_autohide_signal_set = false;
    int m_animation_state = DEF_SHOW;
    gulong m_geometry_change_id = 0;
    Glib::Timer m_animation_timer;
} autohide_static_t;

class Panel : public Gtk::DrawingArea
{
  public:
    Panel();
    ~Panel();

    void init();
    int get_required_size();

    // Signal handler:
    void on_appupdater_update(bool a, int b);

  protected:
    static autohide_static_t m_autohide_static_type;
    static Panel* m_this;
    bool m_mouse_in = false;
    bool m_animation_running = false;
    float m_animation_time = 0.f;
    static bool m_visible;
    static WnckWindow* m_active_window;
    static void check_intelihide();

    float m_animation_hide_delay = 0.f;

    bool m_connect_draw_signal_set = false;

    AppUpdater m_app_updater;
    bool m_draw_required = false;
    static bool m_mouse_inside;
    int m_current_index = -1;

    // Override default signal handler
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    void draw_panel(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_items(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_timeout_draw();

    sigc::connection m_sigc_draw;
    sigc::connection m_sigc_autohide;
    void connect_draw_signal(bool connect);
    static void connect_autohide_signal(bool connect);

    void get_item_position(const dock_item_type_t item_type, int& x, int& y,
                           int& width, int& height);

    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    bool on_motion_notify_event(GdkEventMotion* event);

    int get_index(const int& mouseX, const int& mouseY);
    bool auto_hide_timer();
    bool intelli_hide_timer();

    static void on_geometry_change(WnckWindow* window, gpointer user_data);

    static void on_active_workspace_changed(
        WnckScreen* screen, WnckWorkspace* previously_active_space,
        gpointer user_data);

    static void on_active_window_changed(WnckScreen* screen,
                                         WnckWindow* previously_active_window,
                                         gpointer user_data);
};

DL_NS_END
