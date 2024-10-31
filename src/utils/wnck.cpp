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
        WnckScreen* m_screen = nullptr;

        void init()
        {
            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_PAGER);

            if (!m_screen) {
                m_screen = wnck_handle_get_default_screen(handle);
            }
        }

        WnckScreen* get_default_screen()
        {
            if (!m_screen) init();
            return m_screen;
        }

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

            if (!wnck_window_is_active(window)) wnck_window_activate(window, ct);
        }

        void focus_window(WnckWindow* window, int event_time)
        {
            WnckWorkspace* ws = wnck_window_get_workspace(window);

            wnck_workspace_activate(ws, event_time);

            if (wnck_window_is_minimized(window)) {
                wnck_window_unminimize(window, event_time);
            }

            wnck_window_activate_transient(window, event_time);
        }

        // Taken from plank code,
        void activate_window_ws(std::vector<WnckWindow*> window_list)
        {
            // bool mini = false;
            g_print("Activate ws\n");

            GdkScreen* screen = gdk_screen_get_default();
            int current_ws_number = gdk_x11_screen_get_current_desktop(screen);
            int event_time = gtk_get_current_event_time();

            // Unminimize minimized windows if there is one or moreen_force_update(m_screen);
            int et = gtk_get_current_event_time();
            for (auto& window : window_list) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                if (wnck_window_is_minimized(window)) {
                    for (auto& w : window_list) {
                        wnck_window_unminimize(w, et);
                    }

                    return;
                }
            }

            // Minimize all windows if this application owns the active window
            for (auto& window : window_list) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                WnckScreen* wckscreen = wnck_window_get_screen(window);

                if (wnck_window_is_active(window) ||
                    window == wnck_screen_get_active_window(wckscreen)) {
                    for (auto& w : window_list) {
                        if (!wnck_window_is_minimized(w)) {
                            wnck_window_minimize(w);
                        }
                    }

                    return;
                }
            }

            // Get all windows on the current workspace in the foreground
            for (auto& window : window_list) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                for (auto& w : window_list) {
                    if (!wnck_window_is_minimized(w)) {
                        focus_window(w, event_time);
                    }
                }
            }
        }

        void activate_window_wsORI(std::vector<WnckWindow*> window_list)
        {
            /*g_print("Activate ws\n");
            WnckScreen* screen = get_default_screen();
            GList* window_l;

            window_list.clear();
            wnck_screen_force_update(screen);

            for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);
                if (!window) continue;

                //  if (!is_valid_window_type(window)) continue;

                if (wnck_window_is_below(window)) exit(1);
                // wnck_Vwindow_minimize(window);
            }*/

            bool mini = false;
            g_print("Activate ws\n");

            GdkScreen* screen = gdk_screen_get_default();
            int current_ws_number = gdk_x11_screen_get_current_desktop(screen);

            //    wnck_screen_force_update(m_screen);
            int ct = gtk_get_current_event_time();
            for (auto& window : window_list) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                mini = wnck_window_is_minimized(window);
                if (mini) {
                    break;
                }
            }

            int count = 0;
            for (auto& window : window_list) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                // if (wnck_window_is_below(window)) exit(1);
                // if (wnck_window_is_above(window)) exit(1);

                if (mini) {
                    wnck_window_activate(window, ct);
                } else {
                    // if (wnck_window_get_state(window) ==
                    // WnckWindowState::WNCK_WINDOW_STATE_MINIMIZED) {
                    // wnck_window_activate(window, ct);
                    // continue;
                    //}

                    wnck_window_minimize(window);
                }

                count++;
            }

            if (!count) {
                for (auto& window : window_list) {
                    if (!wnck_window_is_active(window)) wnck_window_activate(window, ct);
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

                return;
            }

            int ct = gtk_get_current_event_time();
            if (!is_window_on_current_desktop(window)) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (ws != nullptr) {
                    wnck_workspace_activate(ws, ct);
                }
            }

            // if (!wnck_window_is_active(window))
            wnck_window_activate(window, ct);
        }

        void close_window(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            wnck_window_close(window, gtk_get_current_event_time());
        }

        void close_all_windows()
        {
            WnckScreen* screen = get_default_screen();
            GList* window_l;

            wnck_screen_force_update(m_screen);
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
            WnckScreen* screen = get_default_screen();
            GList* window_l;

            wnck_screen_force_update(m_screen);
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

        void minimize_all()
        {
            WnckScreen* screen = get_default_screen();
            GList* window_l;

            wnck_screen_force_update(screen);
            for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);
                if (!window) continue;

                if (!is_valid_window_type(window)) continue;

                wnck_window_minimize(window);
            }
        }

        void unminimize_all()
        {
            WnckScreen* screen = get_default_screen();
            GList* window_l;

            wnck_screen_force_update(m_screen);
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
            WnckScreen* screen = get_default_screen();
            GList* window_l;

            wnck_screen_force_update(m_screen);
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

    }  // namespace wnck
}  // namespace docklight
