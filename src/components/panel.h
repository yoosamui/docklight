
#pragma once

#include <glibmm/timer.h>
#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>
#include "common.h"
#include "components/appupdater.h"
#include "components/autohide.h"
DL_NS_BEGIN

#define DEF_HIDE 0
#define DEF_SHOW 1

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
    Autohide m_autohide;
    void on_autohide_update(int x, int y);

    int m_offset_x = 0;
    int m_offset_y = 0;

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
    void connect_draw_signal(bool connect);

    void get_item_position(const dock_item_type_t item_type, int& x, int& y,
                           int& width, int& height);

    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    bool on_motion_notify_event(GdkEventMotion* event);

    int get_index(const int& mouseX, const int& mouseY);

    static void on_active_window_changed(WnckScreen* screen,
                                         WnckWindow* previously_active_window,
                                         gpointer user_data);
};

DL_NS_END
