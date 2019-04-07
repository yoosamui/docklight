//*****************************************************************
//
//  Copyright (C) Juan R. Gonzalez
//  Created on November 20, 2018  
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

#ifndef MONITORGEOMETRY_H
#define	MONITORGEOMETRY_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <gtkmm.h>
#include "Configuration.h"
#include "AppWindow.h"
#include "DockPanel.h"

class AppWindow;

namespace DockWindow {
    void set_Visibility(bool visible);
    int init(Gtk::Window* window);
    int get_DockWindowWidth();
    int get_DockWindowHeight();
    void get_DockWindowPosition(int& x, int& y);
    guint get_dockWindowStartEndMargin();
    //gint getClientSize();
    void update();
    void update(bool move);
    int reSize(bool forceMove);
    int reSize();
    void hide();
    bool is_Horizontal();
    bool is_Visible();
    namespace Monitor
    {
        GdkRectangle get_geometry();
        GdkRectangle get_workarea();
        void updateGeometry();
        void updateStrut();
        void removeStrut();
    }
}

#endif	/* MONITORGEOMETRY_H */

