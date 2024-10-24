#pragma once

#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <glibmm/timer.h>
#include <gtkmm/window.h>
#include <libwnck/libwnck.h>
//#include "DockItem.h"
#include <X11/X.h>
#include <gdk/gdkx.h>

#include "constants.h"

namespace docklight
{

    namespace wnck
    {
        void activate_window_ws(std::vector<WnckWindow*> window_list);
        void activate_window_ws(WnckWindow* window);
        WnckWindow* get_active();

        void bring_window(WnckWindow* window);
        void get_docks();
        void activate_window(WnckWindow* window);
        bool is_window_on_current_desktop(WnckWindow* window);
        std::string get_window_name(WnckWindow* window);
        int get_windows_count();
        void close_all_windows();
        void close_window(WnckWindow* window);
        void minimize(WnckWindow* window);
        void unminimize(WnckWindow* window);
        bool is_valid_window_type(WnckWindow* window);
        int get_workspace_number(WnckWindow* window);
        void HomeMinimizeAll();
        void HomeUnMinimizeAll();
    }  // namespace wnck
}  // namespace docklight

