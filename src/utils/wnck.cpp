#include "wnck.h"

DL_NS_BEGIN

namespace wnck_util
{
    void activate_window(WnckWindow* window)
    {
        int ct = gtk_get_current_event_time();

        if (window == nullptr) return;

        if (!WNCK_IS_WINDOW(window)) {
            return;
        }

        if (wnck_window_is_active(window)) {
            wnck_window_minimize(window);
            return;
        }

        if (!is_window_on_current_desktop(window)) {
            WnckWorkspace* ws = wnck_window_get_workspace(window);
            if (ws != nullptr) {
                wnck_workspace_activate(ws, ct);
            }
        }

        wnck_window_activate(window, ct);
    }

    bool is_window_on_current_desktop(WnckWindow* window)
    {
        GdkScreen* screen = gdk_screen_get_default();
        guint32 current_ws_number = gdk_x11_screen_get_current_desktop(screen);
        WnckWorkspace* ws = wnck_window_get_workspace(window);
        if (ws == nullptr ||
            wnck_workspace_get_number(ws) != (int)current_ws_number) {
            return false;
        }

        return true;
    }

    string get_window_name(WnckWindow* window)
    {
        return string(wnck_window_get_name(window));
    }

}  // namespace wnck_util

DL_NS_END
