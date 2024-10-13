#pragma once
// clang-format off
#include <glibmm/object.h>
#include <gdkmm/pixbuf.h>
#include <libwnck/libwnck.h>
#include <sigc++/sigc++.h>

#include "components/dockitemprovider.h"
#include "components/position.h"
#include "utils/wnck.h"

// clang-format on
namespace docklight
{

    class AppObserver : public Glib::Object
    {
      public:
        AppObserver();

      private:
        static bool valid_type(WnckWindow* window);

        static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);
        static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    };

    Glib::RefPtr<AppObserver> create_observer();
    Glib::RefPtr<AppObserver> Observer();

}  // namespace docklight
