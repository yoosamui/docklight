#include "wnck.h"
#include "components/appupdater.h"

DL_NS_BEGIN

namespace wnck_util
{
    void activate_window(WnckWindow* window)
    {
        if (!WNCK_IS_WINDOW(window)) {
            return;
        }

        if (wnck_window_is_active(window)) {
            wnck_window_minimize(window);

            return;
        }

        int ct = gtk_get_current_event_time();
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
        if (!WNCK_IS_WINDOW(window)) {
            return;
        }

        //  wnck_window_minimize(window);
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

            if (!is_valid_window_type(window)) {
                continue;
            }

            wnck_window_close(window, gtk_get_current_event_time());
        }
    }

    int get_workspace_number(WnckWindow* window)
    {
        if (!WNCK_IS_WINDOW(window)) {
            return false;
        }

        GdkScreen* screen = gdk_screen_get_default();
        guint32 current_ws_number = gdk_x11_screen_get_current_desktop(screen);
        WnckWorkspace* ws = wnck_window_get_workspace(window);
        if (ws == nullptr || wnck_workspace_get_number(ws) != (int)current_ws_number) {
            return -1;
        }

        return current_ws_number;
    }

    bool is_window_on_current_desktop(WnckWindow* window)
    {
        if (!WNCK_IS_WINDOW(window)) {
            return false;
        }

        GdkScreen* screen = gdk_screen_get_default();
        guint32 current_ws_number = gdk_x11_screen_get_current_desktop(screen);
        WnckWorkspace* ws = wnck_window_get_workspace(window);
        if (ws == nullptr || wnck_workspace_get_number(ws) != (int)current_ws_number) {
            return false;
        }

        return true;
    }

    string get_window_name(WnckWindow* window)
    {
        if (!WNCK_IS_WINDOW(window)) {
            return {};
        }

        return string(wnck_window_get_name(window));
    }

    int get_windows_count()
    {
        int count = 0;
        WnckScreen* screen;
        GList* window_l;

        screen = wnck_screen_get_default();

        for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
             window_l = window_l->next) {
            WnckWindow* window = static_cast<WnckWindow*>(window_l->data);

            if (!WNCK_IS_WINDOW(window)) {
                continue;
            }

            if (window == nullptr) {
                continue;
            }

            if (!is_valid_window_type(window)) {
                continue;
            }

            count++;
        }

        return count;
    }

    bool is_valid_window_type(WnckWindow* window)
    {
        if (!WNCK_IS_WINDOW(window)) {
            return false;
        }

        WnckWindowType wt = wnck_window_get_window_type(window);
        // clang-format off
            if (wt == WNCK_WINDOW_DESKTOP ||
                wt == WNCK_WINDOW_DOCK ||
                wt == WNCK_WINDOW_TOOLBAR ||
                wt == WNCK_WINDOW_MENU ||
                wt == WNCK_WINDOW_SPLASHSCREEN) {
                return false;
            }
        // clang-format on

        const char* instancename = wnck_window_get_class_instance_name(window);
        if (instancename != nullptr && strcasecmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
            return false;
        }

        return true;
    }

    void minimize_group(int index)
    {
        if (index == -1) return;

        auto const item = AppUpdater::m_dockitems[index];

        for (auto const citem : item->m_items) {
            WnckWindow* window = citem->get_wnckwindow();
            wnck_window_minimize(window);
        }
    }

    void unminimize_group(int index)
    {
        if (index == -1) return;

        auto const item = AppUpdater::m_dockitems[index];

        for (auto const citem : item->m_items) {
            WnckWindow* window = citem->get_wnckwindow();
            int ct = gtk_get_current_event_time();
            wnck_window_unminimize(window, ct);
        }
    }
}  // namespace wnck_util

DL_NS_END
