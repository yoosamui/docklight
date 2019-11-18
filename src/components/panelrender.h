
#pragma once

#include <glibmm/timer.h>
#include <gtkmm/drawingarea.h>
#include "common.h"
#include "components/appupdater.h"
#include "components/autohide.h"
#include "components/config.h"
#include "components/dockmenu.h"
#include "components/titlewindow.h"

DL_NS_BEGIN

typedef struct {
    void* m_this;
    bool m_mouse_inside = false;
    int m_active_window_index = -1;
    bool m_dragdrop_begin = false;
    int m_decrease_factor = 0;
    bool m_connect_draw_signal_set = false;
    sigc::connection m_sigc_draw;
    bool m_draw_required = false;
    bool m_bck_thread_run = true;
    bool m_bck_thread_connect = false;

} panel_static_members_t;

class Panel_render : public Dock_menu
{
  public:
    Panel_render();
    void init();

    static panel_static_members_t m_stm;

  protected:
    Autohide m_autohide;
    title_window m_titlewindow;
    bool m_titlewindow_visible = false;
    Glib::Timer m_title_timer;
    int m_title_item_index = -1;

    config::style::Theme m_theme;

    int m_offset_x = 0;
    int m_offset_y = 0;

    int m_inverted_index = -1;
    int m_drop_index = -1;
    bool m_show_selector = true;

    void load_home_icon(int icon_size);

    void draw(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_panel(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_items(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_title();

    inline void draw_cell(const Cairo::RefPtr<Cairo::Context>& cr, const shared_ptr<DockItem>& item,
                          int x, int y, int width, int height, int area,
                          Gtk::Orientation orientation) const;

    inline void draw_indicator(const Cairo::RefPtr<Cairo::Context>& cr,
                               const shared_ptr<DockItem>& item, int area, int x, int y, int width,
                               int height) const;

    inline void draw_active_window_indicator(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y,
                                             int idx, int width, int height) const;

    inline void draw_drag_indicator(const Cairo::RefPtr<Cairo::Context>& cr,
                                    const shared_ptr<DockItem>& item, int x, int y, int idx,
                                    int width, int height, Gtk::Orientation orientation) const;

    inline void draw_separator(const Cairo::RefPtr<Cairo::Context>& cr,
                               const shared_ptr<DockItem>& item, const int x, const int y,
                               const int width, const int height,
                               const Gtk::Orientation orientation);

    inline void draw_icon(const Cairo::RefPtr<Cairo::Context>& cr, const shared_ptr<DockItem>& item,
                          const Glib::RefPtr<Gdk::Pixbuf>& icon, int icon_size, int area, int idx,
                          int x, int y);

    inline void get_item_position(const dock_item_type_t item_typ, int& x, int& y, int& width,
                                  int& height);
};

DL_NS_END

