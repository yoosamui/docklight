#pragma once

// clang-format off
#include <gtkmm/window.h>

#include "components/device.h"
#include "components/config.h"
#include "utils/struts.h"
// clang-format on

namespace docklight::position
{

    class PositionManager : public Glib::Object
    {
      public:
        PositionManager(Gtk::Window* window);

        const Gtk::Window* get_window() const;
        Gdk::Rectangle get_workarea() const;
        Gdk::Rectangle get_background_region() const;

        void set_position(guint required_size);

      private:
        Struts m_struts;
        Gtk::Window* m_window;
        guint m_last_required_size = 0;
    };

    Glib::RefPtr<PositionManager> create(Gtk::Window* window);
    Glib::RefPtr<PositionManager> get();

}  // namespace docklight::position
