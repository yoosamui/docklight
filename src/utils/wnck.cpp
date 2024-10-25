#include "wnck.h"
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Translation messages
//
// #define _(String) gettext(String)
// redefined in /usr/include/glib-2.0/glib/gi18n.h
//
// Macro Expansion instead of using String literals:
//
// #define ready_to_go _("Ready to go")
//
// When the preprocessor encounters a macro definition like #define ready_to_go _("Ready to go"),
// it replaces all occurrences of ready_to_go with the text _("Ready to go") before compilation.
// This means that the gettext function is called during the preprocessing stage, and the
// translation is performed.

namespace docklight
{

    namespace wnck
    {
        void bring_window(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
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

        void activate_window_ws(std::vector<WnckWindow*> window_list)
        {
            bool mini = false;

            GdkScreen* screen = gdk_screen_get_default();
            int current_ws_number = gdk_x11_screen_get_current_desktop(screen);

            int ct = gtk_get_current_event_time();
            for (auto& window : window_list) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                mini = wnck_window_is_minimized(window);
                if (mini) break;
            }

            int count = 0;
            for (auto& window : window_list) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                if (mini)
                    wnck_window_activate(window, ct);
                else
                    wnck_window_minimize(window);

                count++;
            }

            if (!count) {
                for (auto& window : window_list) {
                    // if (mini)
                    wnck_window_activate(window, ct);
                    // else
                    //     wnck_window_minimize(window);
                }
            }
        }

        void activate_window_ws(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            GdkScreen* screen = gdk_screen_get_default();
            int current_ws_number = gdk_x11_screen_get_current_desktop(screen);
            WnckWorkspace* ws = wnck_window_get_workspace(window);

            if (wnck_workspace_get_number(ws) != current_ws_number) return;

            if (!wnck_window_is_minimized(window)) {
                //  if (wnck_window_is_active(window)) {
                //  if (wnck_window_is_unmaximize(window)) {
                //  g_print("IN WS %d\n", current_ws_number);
                wnck_window_minimize(window);
                return;
            }
            int ct = gtk_get_current_event_time();
            if (!wnck_window_is_active(window)) {
                wnck_window_activate(window, ct);
            }
        }

        void activate_window(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            if (wnck_window_is_active(window)) {
                wnck_window_minimize(window);
                // if (wnck_window_is_maximized(window)) {
                // wnck_window_unmaximize(window);
                // return;
                //}

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

        void close_window(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            //  wnck_window_minimize(window);
            wnck_window_close(window, gtk_get_current_event_time());
        }

        void get_docks()
        {
            WnckScreen* screen;
            GList* window_l;

            // WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_PAGER);
            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_APPLICATION);
            screen = wnck_handle_get_default_screen(handle);

            wnck_screen_force_update(screen);

            g_print("BEFORE LOOP\n");
            for (window_l = wnck_screen_get_windows(screen); window_l != NULL;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);
                //  g_print("IN LOOP\n");

                // if (!is_valid_window_type(window)) {
                // continue;
                //}

                WnckWindowType wt = wnck_window_get_window_type(window);
                if (wt == WNCK_WINDOW_DOCK || wt == WNCK_WINDOW_TOOLBAR) {
                    int yp = 0;
                    int xp = 0;
                    int widthp = 0;
                    int heightp = 0;

                    wnck_window_get_geometry(window, &xp, &yp, &widthp, &heightp);
                    const char* instance_name = wnck_window_get_class_instance_name(window);
                    g_print(">>[ %s ] %d %d - %d %d\n", instance_name, xp, yp, widthp, heightp);
                }

                //               wnck_window_close(window, gtk_get_current_event_time());
            }
        }
        void close_all_windows()
        {
            WnckScreen* screen;
            GList* window_l;

            // WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_PAGER);
            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_APPLICATION);
            screen = wnck_handle_get_default_screen(handle);
            //            screen = wnck_screen_get_default();
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

        std::string get_window_name(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return {};
            }

            return std::string(wnck_window_get_name(window));
        }

        int get_windows_count()
        {
            int count = 0;
            WnckScreen* screen;
            GList* window_l;

            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_APPLICATION);
            screen = wnck_handle_get_default_screen(handle);

            wnck_screen_force_update(screen);

            for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
                 window_l = window_l->next) {
                WnckWindow* window = static_cast<WnckWindow*>(window_l->data);

                if (!WNCK_IS_WINDOW(window)) {
                    continue;
                }

                if (!window) {
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

            return true;
        }

        void minimize(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            wnck_window_minimize(window);
        }

        void unminimize(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            int ct = gtk_get_current_event_time();
            wnck_window_unminimize(window, ct);
        }

        void HomeMinimizeAll()
        {
            WnckScreen* screen;
            GList* window_l;

            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_APPLICATION);
            screen = wnck_handle_get_default_screen(handle);

            wnck_screen_force_update(screen);

            for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);
                if (!window) continue;

                if (!is_valid_window_type(window)) continue;

                wnck_window_minimize(window);
            }
        }

        void HomeUnMinimizeAll()
        {
            WnckScreen* screen;
            GList* window_l;

            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_APPLICATION);
            screen = wnck_handle_get_default_screen(handle);

            wnck_screen_force_update(screen);

            for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);
                if (!window) continue;

                if (!is_valid_window_type(window)) continue;

                auto ct = gtk_get_current_event_time();
                if (wnck_window_is_minimized(window)) wnck_window_unminimize(window, ct);

                wnck_window_activate(window, ct);
            }
        }

        WnckWindow* get_active()
        {
            WnckScreen* screen;
            GList* window_l;

            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_APPLICATION);
            screen = wnck_handle_get_default_screen(handle);

            wnck_screen_force_update(screen);

            for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);
                if (!window) continue;

                if (!is_valid_window_type(window)) continue;

                if (wnck_window_is_active(window)) {
                    return window;
                }
            }

            return nullptr;
        }

        void HomeCloseAllWindows()
        {
            WnckScreen* screen;
            GList* window_l;

            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_APPLICATION);
            screen = wnck_handle_get_default_screen(handle);

            wnck_screen_force_update(screen);

            for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);
                if (!window) continue;

                if (!is_valid_window_type(window)) continue;

                const char* instancename = wnck_window_get_class_instance_name(window);
                if (instancename != NULL && strcmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                    continue;
                }

                wnck_window_close(window, gtk_get_current_event_time());
            }
        }
    }  // namespace wnck
}  // namespace docklight
