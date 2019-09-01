#pragma once

#include <glibmm/timer.h>
#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>
#include <thread>
#include "common.h"
#include "components/appupdater.h"
#include "components/autohide.h"
#include "components/config.h"
#include "components/dockmenu.h"
#include "components/launcherwindow.h"
#include "components/titlewindow.h"

DL_NS_BEGIN

class Panel : public Gtk::DrawingArea, DockMenu
{
  public:
    Panel();
    ~Panel();
    void set_owner(Gtk::Window* window);
    void init();
    int get_required_size();
    void on_appupdater_update();
    static int m_decrease_factor;

  private:
    config::style::Theme m_theme;
    AppUpdater m_appupdater;
    Gtk::Window* m_owner = nullptr;
    launcher_window m_launcherwindow;
    title_window m_titlewindow;
    Autohide m_autohide;
    void on_autohide_update(int x, int y);

    int m_offset_x = 0;
    int m_offset_y = 0;

    int m_inverted_index = -1;
    int m_drop_index = -1;
    bool m_show_selector = true;
    bool m_connect_draw_signal_set = false;
    bool m_enter_anchor = false;

    void reset_flags();

    bool m_dragdrop_begin = false;
    Glib::Timer m_dragdrop_timer;

    bool m_draw_required = false;
    static bool m_mouse_inside;
    int m_current_index = -1;
    bool m_mouse_left_down = false;
    bool m_mouse_right_down = false;
    void open_new();
    void activate();

    // Override default signal handler
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    void draw_panel(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_items(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_title();
    void draw();

    Glib::Timer m_title_timer;
    int m_title_item_index = -1;

    bool on_timeout_draw();

    sigc::connection m_sigc_draw;
    void connect_draw_signal(bool connect);

    int get_index(const int& mouseX, const int& mouseY);

    void get_item_position(const dock_item_type_t item_type, int& x, int& y, int& width,
                           int& height);

    bool get_center_position(int& x, int& y, const int width, const int height);

    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);

    // Mouse handlers
    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    bool on_motion_notify_event(GdkEventMotion* event);
    bool on_scroll_event(GdkEventScroll* e);

    static void on_active_window_changed(WnckScreen* screen, WnckWindow* previously_active_window,
                                         gpointer user_data);
    // Menus
    bool m_context_menu_open = false;

    void on_home_menu_addseparator_event();
    void on_home_menu_quit_event();

    void on_menu_hide_event();
    void on_menu_show_event();

    void on_home_menu_position(int& x, int& y, bool& push_in);
    void on_item_menu_position(int& x, int& y, bool& push_in);
    void on_separator_menu_position(int& x, int& y, bool& push_in);
    void on_item_menu_windowlist_position(int& x, int& y, bool& push_in);
    void on_item_menu_new_event();
    void on_item_menu_attach_event();
    void on_separator_menu_attach_event();
    void on_item_menu_windowlist_event(WnckWindow* window);
};

DL_NS_END
