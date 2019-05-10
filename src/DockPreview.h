//*****************************************************************
//
//  Copyright © 2018 Juan R. González
//  j-gonzalez@email.de
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//*****************************************************************


#ifndef DOCKPREVIEW_H
#define DOCKPREVIEW_H

#include <gtkmm.h>
#include "Configuration.h"
#include "DockItem.h"

class DockPreview : public Gtk::Window
{
    public:
        DockPreview();
        ~DockPreview();
        void init(const std::vector<DockItem*>& items, const guint index);
    private:
        bool m_canLeave = false;
        bool m_mouseIn = false;

        guint cellWidth;
        guint cellHeight;

        void hideMe();
        bool on_leave_notify_event(GdkEventCrossing* crossing_event);
        bool on_enter_notify_event(GdkEventCrossing* crossing_event);

        Configuration::Style::Theme m_Theme = Configuration::get_Theme();
        Pango::FontDescription m_font;
        bool on_timeoutDraw();
        static std::vector<DockItem*> m_previewItems;
        static void on_window_opened(WnckScreen *screen, WnckWindow *window, gpointer data);
        static void on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data);
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
};

#endif /* DOCKPREVIEW_H */
