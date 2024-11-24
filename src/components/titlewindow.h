#pragma once

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

//#include "common.h"
#include "components/config.h"

namespace docklight
{

    class TitleWindow : public Gtk::Window
    {
      public:
        TitleWindow();
        ~TitleWindow();
        void set_text(const Glib::ustring text);
        void show_at(int x, int y);
        void hide_now();

      private:
        bool m_visible = false;
        std::string m_last_text;
        //      config::style::Theme m_theme;
        // Configuration::Style::Theme m_Theme = Configuration::get_Theme();
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
        Pango::FontDescription font;

        // Child widgets:
        Gtk::Box m_HBox;
        Gtk::Label m_Label;
    };
}  // namespace docklight

