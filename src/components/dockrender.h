#pragma once

#include <gdkmm/general.h>
//#include <glibmm/main.h>
#include <gtkmm/window.h>
//  clang-format off
#include <glibmm/main.h>
#include <gtkmm/drawingarea.h>

#include "components/config.h"
#include "components/dockitemcontainer.h"
#include "components/position.h"
//  clang-format on

namespace docklight
{

    class DockRender : public Gtk::DrawingArea
    {
      public:
        DockRender();
        virtual ~DockRender();

      protected:
      protected:
      private:
        void create_surface_background();
        void draw_background();

        void create_surface_cell();
        void draw_surface_cell();

        void create_surface_icon();

        void draw_surface_icon(const Glib::RefPtr<DockItem>& item);
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

      private:
        guint m_posX = 0;
        guint m_posY = 0;

        Cairo::RefPtr<Cairo::ImageSurface> m_cell;
        Cairo::RefPtr<Cairo::ImageSurface> m_surface_icon;
        Cairo::RefPtr<Cairo::ImageSurface> m_indicator;
        Cairo::RefPtr<Cairo::ImageSurface> m_background;

        Cairo::RefPtr<Cairo::Context> m_icon_ctx;
        Cairo::RefPtr<Cairo::Context> m_bck_ctx;
        Cairo::RefPtr<Cairo::Context> m_cell_ctx;
    };

}  // namespace docklight
