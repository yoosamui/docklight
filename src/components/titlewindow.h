#pragma once

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>
#include "common.h"
#include "components/config.h"

DL_NS_BEGIN

class title_window : public Gtk::Window
{
  public:
    title_window();
    ~title_window();
    void set_text(const Glib::ustring text);

  private:
    // Configuration::Style::Theme m_Theme = Configuration::get_Theme();
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    Pango::FontDescription font;

    // Child widgets:
    Gtk::Box m_HBox;
    Gtk::Label m_Label;
};

DL_NS_END

