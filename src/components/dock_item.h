#pragma once

//#include <gtkmm/drawingarea.h>
//#include <gtkmm/widget.h>
#include <gtkmm/image.h>
#include "common.h"

DL_NS_BEGIN

class dock_item : public Gtk::Image
{
  public:
    dock_item();
    void set_image(const Glib::RefPtr<Gdk::Pixbuf>& image);

  protected:
    //    Glib::RefPtr<Gdk::Pixbuf> m_image;
    //    bool on_timeout();

    // Override default signal handler
    //    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

DL_NS_END

