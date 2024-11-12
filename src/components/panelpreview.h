#pragma once
//  Copyright (c) 2018-2024 Juan R. González
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
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <thread>
#include <glibmm/main.h>
#include <gtkmm/window.h>
#include "utils/cairo.h"

#include "utils/wnck.h"
#include "utils/pixbuf.h"
#include "components/TransparentWindow.h"
#include "components/dockitemicon.h"
// clang-format on

namespace docklight
{
    class PanelPreview : public Gtk::Window  // TransparentWindow
    // class PanelPreview : public TransparentWindow
    {
      public:
        PanelPreview();
        ~PanelPreview();

        void show_at(int x, int y, std::shared_ptr<DockItemIcon> dockitem);
        void hide_now();

        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
        bool get_visible() const;

        guint get_width();

      private:
        void connect_signal(bool connect);
        bool on_timeout_draw();

      private:
        sigc::connection m_sigc_connection;
        Glib::RefPtr<Gdk::Pixbuf> m_image;
        std::vector<Glib::RefPtr<Gdk::Pixbuf>> m_current_images;
        std::shared_ptr<DockItemIcon> m_dockitem;

        int m_size = 0;
        bool m_visible = false;
    };

}  // namespace docklight
