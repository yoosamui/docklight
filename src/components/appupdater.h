#pragma once
// clang-format off
//#include <glibmm/main.h>
#include <glibmm/object.h>
#include <gdkmm/pixbuf.h>
#include <libwnck/libwnck.h>

#include <sigc++/sigc++.h>

#include "components/dockitemprovider.h"
#include "components/position.h"

#include "utils/wnck.h"
////#include "constants.h"
//#include <X11/X.h>
//#include <gdk/gdkx.h>
//#include <gtkmm/icontheme.h>
//#include <giomm/appinfo.h>
//#include <giomm/desktopappinfo.h>
//#include <memory>
//#include <vector>
////#include "/usr/include/libbamf3/libbamf/libbamf.h"
//#include <glib.h>
//#include <glib-object.h>
//#include <libbamf/libbamf.h>

//#include "utils/singletonbase.h"
//#include <iostream>


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

    class AppProvider : public Glib::Object
    {
      public:
        AppProvider();

      private:
        static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);
        static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    };

}  // namespace docklight
