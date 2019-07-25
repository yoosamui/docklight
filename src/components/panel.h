
#pragma once

#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>
#include "common.h"
#include "components/appupdater.h"

#include <glibmm/timer.h>
DL_NS_BEGIN

class Panel : public Gtk::DrawingArea
{
  public:
    Panel();
    ~Panel();

    int get_required_size();

    // Signal handler:
    void on_appupdater_update(bool a, int b);

  protected:
    bool m_mouse_in = false;
    bool m_animation_running = false;
    bool m_animation_hidde = false;
    float m_animation_time = 0;
    int m_animation_state = 1;

    bool m_visible = true;
    Glib::Timer m_animation_timer;

    bool m_connect_autohidde_signal_set = false;
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
    sigc::connection m_sigc_autohidde;
    void connect_draw_signal(bool connect);
    void connect_autohidde_signal(bool connect);

    void get_item_position(const dock_item_type_t item_type, int& x, int& y,
                           int& width, int& height);

    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    bool on_motion_notify_event(GdkEventMotion* event);

    int get_index(const int& mouseX, const int& mouseY);
    bool auto_hidde_timer();
};

DL_NS_END
