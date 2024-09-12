#pragma once

// clang-format off
//
//#include <cairomm/context.h>
//#include <gdkmm.h>
#include <glibmm/main.h>
#include <gtkmm/drawingarea.h>

#include "utils/easing.h"
#include <glibmm/timer.h>
//#include <cmath>
//#include <ctime>
#include <math.h>
#include "appupdater.h"
// clang-format on
//
//

namespace docklight
{
    class Panel : public Gtk::DrawingArea
    {
      public:
        Panel();
        ~Panel();
        void init();

      private:
        sigc::connection m_sigc_draw;

        //        Glib::RefPtr<AppProvider> m_app_provider;
        AppProvider m_app_provider;

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
        float m_posY = 0.f;

      private:
        double m_hide_progress = 0.0;

        easing::bounce m_bounce;
        Cairo::RefPtr<Cairo::ImageSurface> m_surfaceIcon;
        Cairo::RefPtr<Cairo::ImageSurface> m_surface;
        Cairo::RefPtr<Cairo::ImageSurface> m_background;
        Cairo::RefPtr<Cairo::ImageSurface> m_main_surface;

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

