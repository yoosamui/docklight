#pragma once

// clang-format off
//
//#include <cairomm/context.h>
//#include <gdkmm.h>
#include <glibmm/main.h>
#include <gtkmm/drawingarea.h>

#include "components/config.h"

#include <gtkmm/widget.h>
#include "utils/easing.h"
#include <glibmm/timer.h>
//#include <cmath>
//#include <ctime>
#include <math.h>
#include "appupdater.h"
#include "dockitemcontainer.h"
//#include <gtkmm/widget.h>
//#include <gtkmm/frame.h>
// clang-format on
//
//

#include "components/TransparentWindow.h"

namespace docklight
{

    class DockCell : public TransparentWindow
    {
      public:
        DockCell()
        {
            add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK |
                       Gdk::SMOOTH_SCROLL_MASK | Gdk::POINTER_MOTION_HINT_MASK |
                       Gdk::FOCUS_CHANGE_MASK | Gdk::ENTER_NOTIFY_MASK | Gdk::LEAVE_NOTIFY_MASK |
                       Gdk::POINTER_MOTION_HINT_MASK | Gdk::POINTER_MOTION_MASK);
        }

        bool on_button_press_event(GdkEventButton* event)
        {
            //
            g_print("WIDGET PRESS\n");
        }

        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override
        {
            g_print("CELL DRAW\n");
            cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
            cr->paint();
            return false;
        }

      public:
    };

    namespace cairo
    {

        // cairo_surface_t* m_cobject = nullptr;
        //    bool has_reference = false;

        //// Glib::RefPtr<DockItem>(new DockItem(xid, instance_name, group_name, wintype));
        //// return Glib::RefPtr<Surface>(new Surface(m_object, true));
        // return Glib::RefPtr<Surface>(new Surface());
        //}

        // Glib::RefPtr<Surface>(new Surface());

        class Surface : public Glib::Object
        // Cairo::ImageSurface  // Glib::Object  //./Cairo::ImageSurface
        {
            // cairo_surface_t* m_cobject;

            // Cairo::RefPtr<Cairo::ImageSurface> m_surface;
            // Cairo::RefPtr<Cairo::Context> m_context;

          public:
            struct Member {
                int value;
                Cairo::RefPtr<Cairo::ImageSurface> surface;
                Cairo::RefPtr<Cairo::Context> context;
            };

            Member m_member;
            /*Surface(cairo_surface_t* cobject) : Cairo::ImageSurface(cobject)
            {
                //
            }*/

            // const Cairo::RefPtr<Cairo::ImageSurface>& operator->()
            //{
            ////
            // return m_surface;
            //}

            // Cairo::RefPtr<Cairo::ImageSurface>& operator->(
            // Cairo::RefPtr<Cairo::ImageSurface>& m_context);

            //
            // return this;

            Member* operator->()
            {
                //
                return &m_member;
            }

            bool operator!()
            {
                //
                return m_member.surface ? true : false;
            }
            /*
                        Cairo::RefPtr<Cairo::Context> operator->()
                        {
                            return m_context;
                            // return this;
                        }

                        Cairo::RefPtr<Cairo::ImageSurface> operator->()
                        {
                            return m_surface;
                            // return this;
                        }
           */
            Surface() {}
            void create(int width, int height)
            {
                m_member.surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
                m_member.context = Cairo::Context::create(m_member.surface);
            }

            Cairo::RefPtr<Cairo::Context> ctx() { return Cairo::Context::create(m_member.surface); }
            // void create(int width, int height)
            //{
            // g_print("SURFACE CREATED");

            // if (!m_member.surface) {
            // m_member.surface =
            // Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
            //}
            // if (!m_member.context) {
            // m_member.context = Cairo::Context::create(m_member.surface);
            //}
            //}

            // Cairo::RefPtr<Cairo::Context> context()
            //{
            // return m_context ? m_context : (m_context = Cairo::Context::create(m_surface));
            //}
            // const Cairo::RefPtr<Cairo::ImageSurface> sobj() const
            //{  //
            // return m_surface;
            //}

            // Surface(cairo_surface_t* cobject) : Cairo::ImageSurface(cobject)
            //{
            ////
            //}
            // ImageSurfacepcreate(Format format, int width, int height);

            // Surface bool button_press(GdkEventButton* event) const
            //{
            ////   if ( even->x )//
            // return true;
            //}
            // V      const Cairo::RefPtr<Cairo::ImageSurface>& get() const
            //{
            // return m_surface;  //
            //}
            //
            /*Cairo::RefPtr<Cairo::ImageSurface> create(int size)
            {
                return Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, size, size);
            }*/

          private:
            //        Cairo::RefPtr<Cairo::ImageSurface> m_surface;

          private:
            // bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override
            //{
            ////
            //}
        };

        // Surface/* Glib::RefPtr<Surface>*/ void CreateSurface(int width, int height);
        // Glib::RefPtr<Surface> void CreateSurface();

        /*Glib::RefPtr<Surface> void CreateSurface()
       {
           //  cairo_surface_t * cobject);  //: Cairo::ImageSurface(cobject)
           auto t = Glib::RefPtr<Surface>(new Surface());
       }*/
    }  // namespace cairo

    class Panel : public Gtk::DrawingArea
    {
      public:
        Panel();
        ~Panel();
        void init();

        // signal_button_press_event().connect([this](GdkEventButton* p_event) {
        // return OnButtonPressed(p_event);
        //});

      private:
        sigc::connection m_sigc_draw;

        sigc::connection m_sigc_updated;

        //        Glib::RefPtr<AppProvider> m_app_provider;
        AppProvider m_app_provider;

        //  Glib::RefPtr<DockCell> m_cell;
        bool on_button_press_event(GdkEventButton* event);
        bool on_motion_notify_event(GdkEventMotion* event);
        void on_container_updated(window_action_t action, int index);

        bool on_drawX(const Cairo::RefPtr<Cairo::Context>& cr);
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        double easing_bounce(double t, double d, double n)
        {
            auto p = t / d;
            return std::fabs(std::sin(n * M_PI * p) *
                             std::min(1.0, (1.0 - p) * (2.0 * n) / (2.0 * n - 1.0)));
        }

        long int m_frame_time = 0;  // GLib.get_monotonic_time();

        bool m_mouse_enter = false;
        bool on_enter_notify_event(GdkEventCrossing* crossing_event) override;
        bool on_leave_notify_event(GdkEventCrossing* crossing_event) override;

        Glib::Timer m_animation_timer;
        long int m_LastClicked = 0;  // m_LastCli
        float m_initTime = 0.0f;
        float m_endTime = 0.0f;
        float m_animation_time = 0.0f;
        // float m_posY = 0.f;

        const Cairo::RefPtr<Cairo::ImageSurface> draw_indicator(const Glib::RefPtr<DockItem>& item);
        const Cairo::RefPtr<Cairo::ImageSurface> draw_icon(const Glib::RefPtr<DockItem>& item);
        const Cairo::RefPtr<Cairo::ImageSurface> create_iconX(const Glib::RefPtr<DockItem>& item);

      private:
        double m_hide_progress = 0.0;

        int m_posX = 0;
        int m_posY = 0;

        easing::bounce m_bounce;
        Cairo::RefPtr<Cairo::ImageSurface> m_surfaceIcon;
        Cairo::RefPtr<Cairo::ImageSurface> m_indicator;
        Cairo::RefPtr<Cairo::ImageSurface> m_surface;
        Cairo::RefPtr<Cairo::ImageSurface> m_background;
        Cairo::RefPtr<Cairo::ImageSurface> m_main_surface;
        Cairo::RefPtr<Cairo::ImageSurface> m_cell;

        cairo::Surface m_back_obj;
        cairo::Surface m_icon_obj;

        void draw_cell();
        void draw_glow();
        void draw_icon();
        void draw_background();
        void aplay_layers();
        void draw_background(const Cairo::RefPtr<Cairo::Context>& cr,
                             Gdk::Rectangle background_rect, int x_offset, int y_offset);

        //  Cairo::RefPtr<Cairo::Context> m_main_ctx;
        void draw_backgroundX(const Cairo::RefPtr<Cairo::Context>& cr,
                              Gdk::Rectangle background_rect, int x_offset, int y_offset);
        void Test()
        {
            //
            //   cairo_surface_t* m_target = nullptr;
            // Cairo::Surface::Surface(m_surface);

            //     cairo_surface_t* result =
            //         cairo_surface_create_for_rectangle(m_target, 10.0, 10.0, 100.0, 100.0);
            //
        }
        /*
         main_buffer = new Surface.with_cairo_surface (win_rect.width, win_rect.height,
    cr.get_target ()); void DrawSurface(cairo_surface_t* surface)
        {
            //
        }
         // Cairo::RefPtr<Cairo::Surface> refSurface  ??;
    // cr->set_source(refSurface, 0, 0);
    cr->paint()
    */

        bool on_timeout_draw();
    };
}  // namespace docklight

