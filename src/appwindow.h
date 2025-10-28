#pragma once
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"

// clang-format off
#include <gtkmm.h>
#include <gdk/gdkx.h>       // For GDK_WINDOW_XDISPLAY, GDK_WINDOW_XID

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <iostream>


#include <gtkmm/application.h>
#include <glibmm/main.h>
#include <gtkmm/window.h>

#include "constants.h"
#include "components/panel.h"
#include "components/device.h"
#include "components/config.h"
#include "components/appobserver.h"
#include "components/dockitemprovider.h"
#include "components/compositewindow.h"
#include "components/animation.h"
#include "components/autohidemanager.h"
#include "config.h"

// clang-format on

namespace docklight
{

    class AppWindow : public CompositeWindow, public IAppWindow
    {
      public:
        AppWindow();
        ~AppWindow();

        IAppWindow* as_interface() { return this; }

        int init(Glib::RefPtr<Gtk::Application>& app);
        // Config* m_config = Config::getInstance();

        void set_window_passthrought(bool passthrough)
        {
#ifdef GDK_WINDOWING_X11
            auto gdk_window = get_window();
            if (!gdk_window) return;

            Display* dpy = GDK_WINDOW_XDISPLAY(gdk_window->gobj());
            ::Window xid = GDK_WINDOW_XID(gdk_window->gobj());

            int w = get_width();
            int h = get_height();
            g_message("%d %d ", w, h);
            // Full window receives input
            Region region = XCreateRegion();
            XRectangle full{0, 0, static_cast<unsigned short>(w), static_cast<unsigned short>(h)};
            XUnionRectWithRegion(&full, region, region);

            if (passthrough) {
                g_message("Passthrough ON");

                {
                    if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                        if (Config()->get_dock_location() == dock_location_t::bottom) {
                            // Subtract small rectangle click-through
                            XRectangle hole{0, 0, static_cast<unsigned short>(w),
                                            static_cast<unsigned short>(h - 10)};
                            Region hole_region = XCreateRegion();
                            XUnionRectWithRegion(&hole, hole_region, hole_region);
                            XSubtractRegion(region, hole_region, region);
                            XDestroyRegion(hole_region);
                        } else {
                            // Subtract small rectangle click-through
                            XRectangle hole{0, 10, static_cast<unsigned short>(w),
                                            static_cast<unsigned short>(h - 10)};
                            Region hole_region = XCreateRegion();
                            XUnionRectWithRegion(&hole, hole_region, hole_region);
                            XSubtractRegion(region, hole_region, region);
                            XDestroyRegion(hole_region);
                        }
                        //
                    } else {
                        // g_message(
                        //"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA "
                        //"ON");
                        if (Config()->get_dock_location() == dock_location_t::left) {
                            // Subtract small rectangle  click-through
                            XRectangle hole{10, 0, static_cast<unsigned short>(w - 10),
                                            static_cast<unsigned short>(h)};
                            //
                            Region hole_region = XCreateRegion();
                            XUnionRectWithRegion(&hole, hole_region, hole_region);
                            XSubtractRegion(region, hole_region, region);
                            XDestroyRegion(hole_region);

                        } else {
                            // Subtract small rectangle  click-through
                            XRectangle hole{0, 0, static_cast<unsigned short>(w - 10),
                                            static_cast<unsigned short>(h)};
                            //
                            Region hole_region = XCreateRegion();
                            XUnionRectWithRegion(&hole, hole_region, hole_region);
                            XSubtractRegion(region, hole_region, region);
                            XDestroyRegion(hole_region);
                        }

                        //
                    }

                    //// Subtract small rectangle (10x10 top-left) → click-through
                    // XRectangle hole{0, 0, static_cast<unsigned short>(w),
                    // static_cast<unsigned short>(h - 10)};
                    /*Region hole_region = XCreateRegion();
                    XUnionRectWithRegion(&hole, hole_region, hole_region);
                    XSubtractRegion(region, hole_region, region);
                    XDestroyRegion(hole_region);*/
                }
            } else {
                g_message("passthrough OFF");
                // Full window receives input → region covers whole window
                XRectangle full{0, 0, static_cast<unsigned short>(w),
                                static_cast<unsigned short>(h)};
                XUnionRectWithRegion(&full, region, region);
            }

            // Apply region as input mask
            XShapeCombineRegion(dpy, xid, ShapeInput, 0, 0, region, ShapeSet);
            XDestroyRegion(region);
            XFlush(dpy);

#endif
        }

      private:
        // instantiate listener
        Glib::RefPtr<Configuration> m_config;
        Glib::RefPtr<AppObserver> m_observer;
        Glib::RefPtr<PositionManager> m_position;
        Glib::RefPtr<DockItemProvider> m_provider;
        Glib::RefPtr<AutohideManager> m_autohide;
        Glib::RefPtr<AnimationManager> m_animation;

        //    bool on_timeout_draw();
        bool on_button_press_event(GdkEventButton* event) override;
        // bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

        //  void createWindow();
        void on_monitor_changed();

        static void send_Notify(const Glib::ustring& title, const Glib::ustring& text,
                                const Glib::ustring& icon_name);
        static int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line,
                                   Glib::RefPtr<Gtk::Application>& app);

        //  void on_container_updated(window_action_t action, gint index);
        static void on_app_activated();
        // static void send_notification(const Glib::ustring& title, const Glib::ustring& text,
        // const Glib::ustring& icon_name);

        // bool on_enter_notify_event(GdkEventCrossing* crossing_event);
        // bool on_leave_notify_event(GdkEventCrossing* crossing_event);

      private:
        // Glib::RefPtr<Gtk::Window> m_window;
        //  sigc::connection m_sigc_updated;
        //  Glib::RefPtr<AnimBoomWindow> m_composite;
        AnimBoomWindow m_composite;
        Panel* m_panel;
        // Panel m_panel;
        static Glib::RefPtr<Gtk::Application> m_application;
    };

}  // namespace docklight
