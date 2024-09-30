#pragma once

// clang-format off
#include <gtkmm/window.h>
#include "components/device.h"
#include "components/config.h"
// clang-format on

namespace docklight::position
{

    class Struts : public Glib::Object
    {
        typedef enum struts_enum {
            left = 0,
            right = 1,
            top = 2,
            bottom = 3,
            left_start = 4,
            left_end = 5,
            right_start = 6,
            right_end = 7,
            top_start = 8,
            top_end = 9,
            bottom_start = 10,
            bottom_end = 11
        } struts_position_t;

      public:
        Struts();
        ~Struts();

        void init(Gtk::Window* window);
        void set_struts(bool create = true);
        void set_insets(long insets[12])
        {
            GtkWidget* toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
            auto gdk_window = gtk_widget_get_window(toplevel);
            if (!gdk_window) {
                g_critical("set_strut: gdk_window is null.");
                return;
            }
            gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                                gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                                (unsigned char*)&insets, 12);

            gdk_property_change(gdk_window, gdk_atom_intern("_NET_WM_STRUT", FALSE),
                                gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                                (unsigned char*)&insets, 4);
        }

      private:
        Gtk::Window* m_window;
        bool m_strut_set = false;
    };

}  // namespace docklight::position
