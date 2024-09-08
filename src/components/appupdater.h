#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1
// clang-format off
#include <glibmm/main.h>
#include <glibmm/object.h>
#include <libwnck/libwnck.h>
#include <sigc++/sigc++.h>
//#include "constants.h"
#include <X11/X.h>
#include <gdk/gdkx.h>

#include <vector>
//#include "/usr/include/libbamf3/libbamf/libbamf.h"
#include <glib.h>
#include <glib-object.h>
#include <libbamf/libbamf.h>
//#include "bamf-view.h"
//#include <libbamf/bamf-view-private.h>
//#include <libbamf/bamf-application.h>
//
//#include <libbamf.h>
//#include "bamf-application.h"
//#include "bamf-window.h"
//#include "bamf-matcher.h"
//#include "bamf-legacy-window.h"
//#include "bamf-legacy-screen.h"
//#include "bamf-tab.h"
//#include <string.h>
//#include <gio/gdesktopappinfo.h>
// clang-format on
namespace docklight
{
    typedef struct {
        // Glib::ustring m_name;
        // Glib::ustring m_group;
        Glib::ustring m_title;
        const gchar* m_window_name = nullptr;
        const gchar* m_group = nullptr;
        const gchar* m_instance_name = nullptr;
        const gchar* m_desktop_file = nullptr;
        unsigned long m_xid = 0;

        // std::string m_name;
        // std::string m_group;
        //  string m_group;
        //  string m_instance;
        //  string m_title;
        //  string m_comment;
        //  string m_locale;
        //   bool m_error = false;
        //   string m_desktop_file;
        //   string m_icon_name;
        //   bool m_cache = false;
        //   WnckWindow* m_wnckwindow = nullptr;
        //   dock_item_type_t m_dock_item_type;
        //   unsigned long m_xid = 0;
        //   Glib::RefPtr<Gdk::Pixbuf> m_image = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;
        //   int m_separator_length = 0;
        //   bool m_separartor_expands = true;
        //   int m_width;
        //   int m_height;
    } appinfo_t;

    class AppProvider : public Glib::Object
    {
      public:
        AppProvider();

      private:
        // appinfo_t m_appinfo;

        //  static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    };

}  // namespace docklight
