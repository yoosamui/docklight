
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

    // Override default signal handler
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

DL_NS_END
