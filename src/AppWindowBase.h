//*****************************************************************
//
//  Copyright (C) 2019 Juan R. Gonzalez
//  j-gonzalez@email.de
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************

#ifndef APPWINDOWBASE_H
#define APPWINDOWBASE_H

#include <gtkmm/window.h>

class AppWindowBase : public Gtk::Window
{
    public:
        AppWindowBase()
        {
            GdkScreen *screen;
            GdkVisual *visual;

            gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
            screen = gdk_screen_get_default();
            visual = gdk_screen_get_rgba_visual(screen);

            if (visual != NULL && gdk_screen_is_composited(screen)) {
                gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
            }
        }
};



#endif /* APPWINDOWBASE_H */
