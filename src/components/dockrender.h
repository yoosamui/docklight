#pragma once
//  Copyright (c) 2018-2024 Juan R. González
//
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  public Glib::Object GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  identification number, along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <gdkmm/general.h>

#include "utils/cairo.h"
#include "components/dockmenu.h"
#include "components/config.h"
#include "components/dockitemprovider.h"
#include "components/position.h"
#include "components/panelhide.h"
// clang-format on

namespace docklight
{
    class DockRender : public DockMenu
    {
      public:
        DockRender();
        virtual ~DockRender();

      private:
        void create_surface_background();
        void draw_surface_background();

        void create_surface_cell();
        void draw_surface_cell(std::shared_ptr<DockItemIcon>& item);

        void create_surface_icon();
        void draw_surface_icon(std::shared_ptr<DockItemIcon>& item);

        void create_surface_indicator(std::shared_ptr<DockItemIcon>& item);
        void draw_surface_indicator(std::shared_ptr<DockItemIcon>& item);

        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

        void on_autohide_update(int x, int y);

      protected:
        Glib::RefPtr<PositionManager> m_position;

        PanelHide m_panel_hide;
        void get_start_pos(gint maxsize, gint& x, gint& y);

      private:
        int m_offsetX = 0;
        int m_offsetY = 0;

        Cairo::RefPtr<Cairo::ImageSurface> m_cell;
        Cairo::RefPtr<Cairo::ImageSurface> m_icon;
        Cairo::RefPtr<Cairo::ImageSurface> m_indicator;
        Cairo::RefPtr<Cairo::ImageSurface> m_background;

        Cairo::RefPtr<Cairo::Context> m_icon_ctx;
        Cairo::RefPtr<Cairo::Context> m_bck_ctx;
        Cairo::RefPtr<Cairo::Context> m_cell_ctx;
        Cairo::RefPtr<Cairo::Context> m_indicator_ctx;
    };

}  // namespace docklight
