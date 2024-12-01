#pragma once

#include <gdkmm/general.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/window.h>

#include "components/position.h"
#include "utils/cairo.h"
//#include "common.h"
#include "components/config.h"

namespace docklight
{

    class DADWindow : public Gtk::Window
    {
      public:
        DADWindow(Glib::RefPtr<Gdk::Pixbuf> icon);
        ~DADWindow();
        void show_at(int dockitem_index);
        void move_at(int x, int y);
        void close_now();

        guint get_x() { return m_x; }
        guint get_y() { return m_y; }

      private:
        // bool on_motion_notify_event(GdkEventMotion* event);

      private:
        Glib::RefPtr<Gdk::Pixbuf> m_icon;

        bool m_visible = false;
        std::string m_last_text;
        //      config::style::Theme m_theme;
        // Configuration::Style::Theme m_Theme = Configuration::get_Theme();
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
        Pango::FontDescription font;

        // Child widgets:
        Gtk::Box m_HBox;
        Gtk::Label m_Label;

        int m_size = 0;
        guint m_x = 0;
        guint m_y = 0;
    };
}  // namespace docklight

