/*
 * File:   WckHandler.cpp
 * Author: yoo
 *
 * Created on March 18, 2019, 6:01 PM
 */

#include "WnckHandler.h"

namespace WnckHandler
{

    void HomeCloseAllWindows()
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

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }


            wnck_window_close(window, gtk_get_current_event_time());

        }
    }

    void HomeCloseAllExceptActive()
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

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            if (wnck_window_is_active(window))
                continue;

            wnck_window_close(window, gtk_get_current_event_time());

        }
    }


void HomeMinimizeAllExceptActive()
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

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            if (wnck_window_is_active(window))
                continue;

            wnck_window_minimize(window);

        }
    }


 void HomeMinimizeAll()
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

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            wnck_window_minimize(window);

        }
    }









/////////////////////////

    /**
     * Active the current Active window
     * @param window
     */
    void ActivateWindow(WnckWindow* window)
    {
        if (window == NULL)
            return;

        if (wnck_window_is_active(window)) {
            wnck_window_minimize(window);
            return;
        }

        int ct = gtk_get_current_event_time();
        wnck_window_activate(window, ct);

        if (wnck_window_is_minimized(window))
            wnck_window_unminimize(window, ct);
    }

    /**
     * check if any active window in cild items exists.
     * if yes the window pointer will return otherwise null.
     * @param the child items.
     * @return the window pointer will return when an active  window is found otherwise null.
     */
    WnckWindow* get_ActiveWindowIfAny(DockItem* item)
    {
        WnckWindow* window = nullptr;

        for (DockItem* citem:item->m_items) {

            if (citem->m_window == nullptr) {
                continue;
            }

            if (!wnck_window_is_active(citem->m_window)) {
                continue;
            }

            window = citem->m_window;
        }

        return window;
    }
}
