/* 
 * File:   WindowControl.cpp
 * Author: yoo
 * 
 * Created on November 6, 2018, 8:38 PM
 */

//#include <libwnck-3.0/libwnck/window.h>

#include "WindowControl.h"
#include "Configuration.h"

namespace WindowControl
{
    //https://developer.gnome.org/libwnck/stable/WnckWindow.html#WnckWindowType
    // ++++

    WnckWindow* get_ExistingWindowDock()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);
        for (window_l = wnck_screen_get_windows(screen);
             window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            auto instance = wnck_window_get_class_instance_name(window);
            auto wname = wnck_window_get_name(window);

            g_print("%s %s\n", instance, wname);
            //
            //            WnckWindowType wt = wnck_window_get_window_type(window);
            //
            //            if (wt != WNCK_WINDOW_DOCK )
            //            {
            //             
            //                continue;
            //            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }
            //  return window;
        }
        return NULL;

    }

    /**
     * Gets the active WnckWindow on screen . May return NULL sometimes, since
     * not all window managers guarantee that a window is always active.
     * @return true if any window is in fullscreen otherwise false;
     */
    bool FullscreenActive()
    {
        WnckWindow *wckwindow = wnck_screen_get_active_window(wnck_screen_get_default());
        if (wckwindow != NULL && wnck_window_is_fullscreen(wckwindow)) {
            return true;
        }

        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
             window_l != NULL; window_l = window_l->next) {

            wckwindow = WNCK_WINDOW(window_l->data);
            if (wckwindow == NULL)
                continue;

            if (wnck_window_is_fullscreen(wckwindow)) {
                return true;
            }
        }
        return false;
    }




    /*
    bool AppWindow::on_timeout()
    {
        return true;
        // force our program to redraw.
        auto win = get_window();
        if (win)
        {
            Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                    get_allocation().get_height());
            win->invalidate_rect(r, false);
        }

        return true;
    }

     */

}

