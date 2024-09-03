#pragma once

// clang-format off
//
//#include <cairomm/context.h>
//#include <gdkmm.h>
#include <glibmm/main.h>
#include <gtkmm/drawingarea.h>

//#include <cmath>
//#include <ctime>

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

      private:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

      private:
        Cairo::RefPtr<Cairo::ImageSurface> m_surfaceIcon;
        Cairo::RefPtr<Cairo::ImageSurface> m_surface;
        const Cairo::RefPtr<Cairo::ImageSurface>& DrawIcon();

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

