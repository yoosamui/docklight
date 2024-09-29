#pragma once

// clang-format off
#include <gtkmm/window.h>

#include "components/config.h"
// clang-format on
//
// TODO: for testing
#include <libwnck/libwnck.h>

#include "utils/wnck.h"

namespace docklight
{

    namespace position
    {

        void get_docks();
        void init(Gtk::Window& window);
        void set_window_position(guint required_size);

        Gdk::Rectangle get_monitor_geometry();
        Gdk::Rectangle get_background_region();
        Gdk::Rectangle get_workarea();

        const Gtk::Window* get_window();

        void update_client_geometry(int& x, int& y, int& width, int& height);
        // namespace strutsX
        //{
        // void set_strut(bool reset);
        //}
        namespace struts
        {
            void set_strut(bool reset);
            void set_strutX(bool reset);

        }  // namespace struts
    }      // namespace position

}  // namespace docklight
