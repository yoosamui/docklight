#pragma once
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

// clang-format off
#include <map>
#include <gdkmm/rectangle.h>
#include <functional>
#include <libbamf/libbamf.h>
#include <libwnck/libwnck.h>
#include <X11/X.h>
#include <gdk/gdkx.h>
#include "constants.h"
// clang-format on

namespace docklight
{

    namespace wnck
    {
        void init();

        WnckScreen* get_default_screen();

        WnckWindow* get_window_by_xid(gulong xid);
        Gdk::Rectangle get_window_geometry(WnckWindow* window);
        Gdk::Rectangle get_window_geometry(gulong xid);

        void set_active_window(WnckWindow* active_window);
        bool is_window_on_current_desktop(WnckWindow* window);

        void move_window_to_workspace(WnckWindow* window);
        void move_to_worksapce_number(int number);

        void select_window(WnckWindow* window);
        void select_window(gulong hash, WnckWindow* active_window,
                           std::vector<WnckWindow*> window_list);

        void activate_window(WnckWindow* window);
        void bring_above_window(WnckWindow* window);

        int count_in_workspace(WnckWindow* window, std::string& result);

        void close_all_windows();
        void close_window(WnckWindow* window);
        void close_window(std::vector<WnckWindow*> windows);
        void minimize(WnckWindow* window);
        void unminimize(WnckWindow* window);
        void minimize_group(std::vector<WnckWindow*> list);
        void maximize_group(std::vector<WnckWindow*> list);
        void minimize_all();
        void unminimize_all();

        WnckScreen* get_default_screen();
        WnckWindow* get_active();

        std::string get_window_name(WnckWindow* window);

        int get_windows_count();
        int get_workspace_number(WnckWindow* window);

        bool is_window_on_current_desktop(WnckWindow* window);
        bool is_valid_window_type(WnckWindow* window);

    }  // namespace wnck
}  // namespace docklight

