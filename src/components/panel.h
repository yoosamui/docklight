
#pragma once

#include <gtkmm/drawingarea.h>
#include "common.h"

DL_NS_BEGIN

class panel : public Gtk::DrawingArea
{
  public:
  protected:
    // Override default signal handler
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

DL_NS_END
