#pragma once

// clang-format off
#include <gtkmm/window.h>

#include "components/config.h"
// clang-format on

namespace docklight
{

    namespace position
    {

        void init(Gtk::Window& window);
        void set_window_position(guint required_size);

        Gdk::Rectangle get_background_region();
        Gdk::Rectangle get_workarea();

        const Gtk::Window* get_window();

        namespace struts
        {
            void set_strut(bool reset);
        }
    }  // namespace position

}  // namespace docklight
