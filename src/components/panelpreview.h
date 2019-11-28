
#pragma once

#include <gtkmm/window.h>
#include <sigc++/sigc++.h>
#include "common.h"
#include "components/config.h"
#include "components/dockitem.h"
#include "utils/cairo.h"

DL_NS_BEGIN

class Panel_preview : public Gtk::Window
{
  public:
    Panel_preview();
    ~Panel_preview();

    void init(int index);
    bool show_preview();

    typedef sigc::signal<void> type_signal_close;
    type_signal_close signal_close();

  private:
    bool m_initialized = false;
    config::style::Theme m_theme;

    Gdk::Rectangle m_close_button_rectangle;

    static constexpr const int PREVIEW_WIDTH_EXTENDED_SIZE = 50;
    static constexpr const int PREVIEW_SPARATOR_SIZE = 8;
    static constexpr const int PREVIEW_START_END_MARGIN = 20;
    static constexpr const int PREVIEW_TITLE_SIZE = 20;
    static constexpr const int PREVIEW_LIMIT_HEIGHT = 48;
    static constexpr const int PREVIEW_LIMIT_WIDTH = 97;

    void draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y, const string& text);

    type_signal_close m_signal_close;
    int m_static_count = 0;
    bool m_mouse_in = false;

    int m_window_width;
    int m_window_height;

    int m_width;
    int m_height;

    int m_current_index = 0;
    int m_old_index = -1;
    int m_dockitem_index = -1;

    static vector<shared_ptr<DockItem>> m_previewitems;
    bool on_timeout_draw();
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    inline int get_index(const int& mouseX, const int& mouseY);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    bool on_enter_notify_event(GdkEventCrossing* crossing_event);

    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    bool on_motion_notify_event(GdkEventMotion* event);

    static void on_active_window_changed(WnckScreen* screen, WnckWindow* previously_active_window,
                                         gpointer user_data);
};

DL_NS_END

