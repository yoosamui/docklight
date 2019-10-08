#include "wnck.h"
#include "components/appupdater.h"

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

    void close_group_windows(int index)
    {
        auto const item = AppUpdater::m_dockitems[index];

        for (auto const citem : item->m_items) {
            close_window(citem->get_wnckwindow());
        }
    }

    void close_window(WnckWindow* window)
    {
        if (!window) return;

        wnck_window_minimize(window);
        wnck_window_close(window, gtk_get_current_event_time());
    }

    void close_all_windows()
    {
        WnckScreen* screen;
        GList* window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen); window_l != NULL;
             window_l = window_l->next) {
            WnckWindow* window = WNCK_WINDOW(window_l->data);
            if (window == NULL) continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP || wt == WNCK_WINDOW_DOCK || wt == WNCK_WINDOW_TOOLBAR ||
                wt == WNCK_WINDOW_MENU) {
                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcasecmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }

            wnck_window_close(window, gtk_get_current_event_time());
        }
    }

    bool is_window_on_current_desktop(WnckWindow* window)
    {
        GdkScreen* screen = gdk_screen_get_default();
        guint32 current_ws_number = gdk_x11_screen_get_current_desktop(screen);
        WnckWorkspace* ws = wnck_window_get_workspace(window);
        if (ws == nullptr || wnck_workspace_get_number(ws) != (int)current_ws_number) {
            return false;
        }

        return true;
    }

    string get_window_name(WnckWindow* window) { return string(wnck_window_get_name(window)); }
    int get_windows_count()
    {
        int count = 0;
        WnckScreen* screen;
        GList* window_l;

        screen = wnck_screen_get_default();

        for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
             window_l = window_l->next) {
            WnckWindow* window = static_cast<WnckWindow*>(window_l->data);
            if (window == nullptr) {
                continue;
            }

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP || wt == WNCK_WINDOW_DOCK || wt == WNCK_WINDOW_TOOLBAR ||
                wt == WNCK_WINDOW_MENU || wt == WNCK_WINDOW_SPLASHSCREEN) {
                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != nullptr && strcasecmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }

            count++;
        }

        return count;
    }

}  // namespace wnck_util

DL_NS_END
