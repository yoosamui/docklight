/* 
 * File:   WckHandler.cpp
 * Author: yoo
 * 
 * Created on March 18, 2019, 6:01 PM
 */

#include "WnckHandler.h"

namespace WnckHandler
{

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
