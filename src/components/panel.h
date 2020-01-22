#pragma once

#include <glibmm/timer.h>
#include <sigc++/sigc++.h>
#include <thread>
#include "common.h"
#include "components/launcherwindow.h"
#include "components/panelrender.h"

DL_NS_BEGIN

class Panel : public Gtk::DrawingArea, public Panel_render
{
  public:
    Panel();
    ~Panel();

    void set_owner(Gtk::Window* window);
    void init();
    int get_required_size();
    void on_appupdater_update(window_action_t action, int index);

  private:
    thread* m_bck_thread = nullptr;
    static void connect_async();
    Gtk::Window* m_owner = nullptr;
    launcher_window m_launcherwindow;
    void on_autohide_update(int x, int y);
    float m_mouse_click_event_time = 0.0f;

    void reset_flags();

    Glib::Timer m_dragdrop_timer;

    // s int m_current_index = -1;
    bool m_mouse_left_down = false;
    bool m_mouse_right_down = false;
    void open_new();
    void activate();

    //    void load_home_icon(int icon_size);
    // Override default signal handler
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    void draw_title();
    void draw();
    void show();
    bool on_timeout_draw();

    void close_preview();
    static void connect_draw_signal(bool connect);

    int get_index(const int& mouseX, const int& mouseY);

    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);

    // Mouse handlers
    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    bool on_motion_notify_event(GdkEventMotion* event);
    bool on_scroll_event(GdkEventScroll* e);

    void stop_dragdrop();

    static void set_active_window_indexp(WnckWindow* window);
    static void on_active_window_changed(WnckScreen* screen, WnckWindow* previously_active_window,
                                         gpointer user_data);
    // Menus

    void build_windowlist_menu(GdkEventButton* event);

    void on_home_menu_addseparator_event() override;
    void on_home_menu_addexpander_event() override;
    void on_home_menu_quit_event() override;

    void on_menu_hide_event() override;
    void on_menu_show_event() override;

    void on_home_menu_position(int& x, int& y, bool& push_in) override
    {
        Dock_menu::on_home_menu_position(x, y, push_in);
    }

    void on_item_menu_position(int& x, int& y, bool& push_in) override
    {
        Dock_menu::on_item_menu_position(x, y, push_in);
    }
    void on_separator_menu_position(int& x, int& y, bool& push_in) override
    {
        Dock_menu::on_separator_menu_position(x, y, push_in);
    }
    void on_item_menu_windowlist_position(int& x, int& y, bool& push_in) override
    {
        Dock_menu::on_item_menu_windowlist_position(x, y, push_in);
    }

    void on_item_menu_new_event() override;
    //    void on_item_menu_attach_event();
    // void on_separator_menu_attach_event();
    void on_item_menu_windowlist_event(WnckWindow* window);
};

DL_NS_END
