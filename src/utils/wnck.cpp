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

#include "wnck.h"

//#include <libbamf/bamf-application.h>

#include <functional>

namespace docklight
{

    namespace wnck
    {
        WnckScreen* m_screen = nullptr;
        std::map<gulong, WnckWindow*> m_actives;
        WnckWindow* m_active_window = nullptr;

        void init()
        {
            WnckHandle* handle = wnck_handle_new(WnckClientType::WNCK_CLIENT_TYPE_PAGER);

            if (!m_screen) {
                m_screen = wnck_handle_get_default_screen(handle);
            }
        }

        void set_active_window(WnckWindow* active_window)
        {
            m_active_window = active_window;
        }

        WnckScreen* get_default_screen()
        {
            if (!m_screen) init();
            return m_screen;
        }

        int count_in_workspace(WnckWindow* source_window, std::string& result)
        {
            GdkScreen* screen = gdk_screen_get_default();
            int current_ws_number = gdk_x11_screen_get_current_desktop(screen);
            int count = 0;
            result = "";

            WnckScreen* wckscreen = get_default_screen();
            GList* window_l;

            for (window_l = wnck_screen_get_windows(wckscreen); window_l != nullptr;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);

                if (!is_valid_window_type(window)) continue;

                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (!ws) continue;
                if (wnck_workspace_get_number(ws) == current_ws_number) continue;

                if (source_window != window) continue;

                result += "[" + std::to_string(wnck_workspace_get_number(ws) + 1) + "] ";
                count++;
            }

            return count;
        }

        std::vector<WnckWindow*> get_ordered_window_list(std::vector<WnckWindow*> windows_vector)
        {
            // GList* window_list;
            std::vector<WnckWindow*> windows;
            // GtkWindow* win = nullptr;

            //      WnckScreen *        wnck_screen_get_default             (void);
            WnckScreen* wnckscreen = get_default_screen();

            wnck_screen_force_update(wnckscreen);
            // for (auto& window : windows_vector) {
            // }

            /*for (auto& w : windows_vector) {
                //  if (wnck_window_get_xid(window) == wnck_window_get_xid(w)) {
                g_print("input %lu\n", wnck_window_get_xid(w));
                //
                // window_list = g_list_append(window_list, window);
            }*/

            // Gets the list of WnckWindow on screen in bottom-to-top order.
            GList* stack = wnck_screen_get_windows_stacked(wnckscreen);

            for (stack = wnck_screen_get_windows(wnckscreen); stack != nullptr;
                 stack = stack->next) {
                WnckWindow* window = WNCK_WINDOW(stack->data);

                // if (!is_valid_window_type(window)) {
                // continue;
                //}

                // g_print("stack %lu\n", wnck_window_get_xid(window));
                for (auto& w : windows_vector) {
                    if (wnck_window_get_xid(window) == wnck_window_get_xid(w)) {
                        //
                        // window_list = g_list_append(window_list, window);
                        windows.push_back(window);
                    }
                }
            }

            return windows;
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

        void select_window(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            int event_time = gtk_get_current_event_time();
            focus_window(window, event_time);
        }

        void select_window(gulong hash, WnckWindow* active_window,
                           std::vector<WnckWindow*> window_list)
        {
            GdkScreen* screen = gdk_screen_get_default();
            int current_ws_number = gdk_x11_screen_get_current_desktop(screen);
            int event_time = gtk_get_current_event_time();

            // TODO calculate
            gulong hash_code = hash + current_ws_number;

            //   auto window_list = get_ordered_window_list(windows);

            // Unminimize minimized windows if there is one or more;
            for (auto& window : window_list) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                WnckWindow* ca_window = nullptr;
                if (m_actives.count(hash_code)) {
                    ca_window = m_actives.at(hash_code);
                }

                if (wnck_window_is_minimized(window) && wnck_window_is_in_viewport(window, ws)) {
                    for (auto& w : window_list) {
                        WnckWorkspace* ws = wnck_window_get_workspace(w);
                        if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                        // if (w == ca_window) continue;

                        wnck_window_unminimize(w, event_time);
                    }

                    if (ca_window) {
                        activate_window(ca_window);
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
                    (window == wnck_screen_get_active_window(wckscreen) &&
                     wnck_window_is_in_viewport(window, ws))) {
                    for (auto& w : window_list) {
                        if (!wnck_window_is_minimized(w)) {
                            WnckWorkspace* ws = wnck_window_get_workspace(w);
                            if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                            if (wnck_window_is_active(w)) {
                                m_actives[hash_code] = w;
                            }

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
                if (wnck_window_is_in_viewport(window, ws)) {
                    WnckWindow* ca_window = nullptr;
                    if (m_actives.count(hash_code)) {
                        ca_window = m_actives.at(hash_code);
                    }

                    for (auto& w : window_list) {
                        if (!wnck_window_is_minimized(w)) {
                            WnckWorkspace* ws = wnck_window_get_workspace(w);
                            if (wnck_workspace_get_number(ws) != current_ws_number) continue;

                            //      if (w == ca_window) continue;

                            focus_window(w, event_time);
                        }
                    }

                    if (ca_window) {
                        activate_window(ca_window);
                    }

                    return;
                }
            }

            for (auto& window : window_list) {
                focus_window(window, event_time);
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

            int event_time = gtk_get_current_event_time();
            if (!is_window_on_current_desktop(window)) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (ws != nullptr) {
                    wnck_workspace_activate(ws, event_time);
                }
            }

            focus_window(window, event_time);
            if (!wnck_window_is_active(window)) {
                wnck_window_activate(window, event_time);
            }
        }

        void move_to_worksapce_number(int number)
        {
            WnckScreen* screen = get_default_screen();
            GList* window_l;

            int event_time = gtk_get_current_event_time();
            wnck_screen_force_update(m_screen);

            for (window_l = wnck_screen_get_windows(screen); window_l != NULL;
                 window_l = window_l->next) {
                WnckWindow* window = WNCK_WINDOW(window_l->data);

                // if (!is_valid_window_type(window)) {
                // continue;
                //}

                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (WNCK_IS_WORKSPACE(ws)) {
                    //    if (!ws) continue;

                    if (wnck_workspace_get_number(ws) != number) continue;

                    g_print("IS IN  WVS\n");
                    // if (ws) {
                    wnck_workspace_activate(ws, event_time);
                    g_print("FONUND WVS");
                    exit(1);
                    return;
                    //}
                }
            }
        }
        void move_window_to_workspace(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            int event_time = gtk_get_current_event_time();
            //     if (!is_window_on_current_desktop(window)) {
            WnckWorkspace* ws = wnck_window_get_workspace(window);
            if (ws) {
                wnck_workspace_activate(ws, event_time);
            }
            //   }
        }

        void bring_above_window(WnckWindow* window)
        {
            if (!WNCK_IS_WINDOW(window)) {
                return;
            }

            int event_time = gtk_get_current_event_time();
            if (!is_window_on_current_desktop(window)) {
                WnckWorkspace* ws = wnck_window_get_workspace(window);
                if (ws != nullptr) {
                    wnck_workspace_activate(ws, event_time);
                }
            }

            // focus_window(window, event_time);
            //  if (!wnck_window_is_active(window)) {
            wnck_window_activate(window, event_time);
            //}
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
