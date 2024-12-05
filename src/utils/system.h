#pragma once
//  Copyright (c) 2018-2024 Juan R. González
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

// clang-format off

#include <X11/Xlib.h>  //-lX11
#include <gdk/gdkx.h>
#include <gtk/gtk.h>  //$$(pkg-config --cflags --libs gtk+-3.0)
// Include gtk
#include <gtk/gtk.h>
#include <glibmm/main.h>
#include <gdkmm/rectangle.h>
#include "constants.h"
#include <X11/extensions/Xrender.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

//#include <X11/extensions/composite.h>

//#ifdef HAVE_XCOMPOSITE
//#include <X11/extensions/Xcomposite.h>
//#endif

//#ifdef HAVE_XDAMAGE
//#include <X11/extensions/Xdamage.h>
//#endif

//#ifdef HAVE_RANDR
//#include <X11/extensions/Xrandr.h>
//#endif

// clang-format on

namespace docklight
{
    namespace system
    {
        // test
        void xdraw_window_image2(gulong xid);
        void xget_render_picture_attributes(XID wId);
        void XDrawOnWidget(GtkWidget* widget);
        void xdraw_window_image(GtkWidget* widget, gulong xid, int width, int height);

        // Xlib
        Gdk::Rectangle xget_window_attributes(XID wId, bool& has_alpha);
        void redirecting_all_toplevel_windows_to_offscreen_pixmaps();
        bool is_composite();
        bool get_mouse_position(int& x, int& y);
        bool is_mutter_window_manager();

        bool file_exists(std::string name);
        std::string file_exists(const std::string& directory, std::string& file_name);
        bool is_directory_exists(const char* directory_name);
        bool create_directory_if_not_exitst(const char* directory_name);

        Glib::ustring get_window_manager_name();
        Glib::ustring get_current_path(const Glib::ustring& str);
        Glib::ustring get_current_user();
        Glib::ustring get_current_path();

        /*
        bool file_exists(const Glib::ustring& name);
        bool create_directory_if_not_exitst(const char* directory_name);

        int execute(const std::string& command_string);

        Glib::ustring get_current_path(const Glib::ustring& str);

        const std::string get_current_user();
        std::string get_window_manager_name();
        std::string file_exists(const std::string& directory, std::string& file_name);*/

    }  // namespace system
}  // namespace docklight
