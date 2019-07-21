
#pragma once

#include <gtkmm/drawingarea.h>
#include <sigc++/sigc++.h>
#include "common.h"
#include "components/appupdater.h"

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
    AppUpdater m_app_updater;
    bool m_draw_required = false;

    // Override default signal handler
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    void draw_panel(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_items(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_timeout_draw();

    sigc::connection m_sigc_connection;

    void get_item_position(const dock_item_type_t item_type, int& x, int& y,
                           int& width, int& height);
};

DL_NS_END
