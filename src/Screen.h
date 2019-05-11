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


#ifndef SCREEN_H
#define	SCREEN_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <gtkmm.h>
#include <gdk/gdk.h>
#include "vector"
#include "Struts.h"


class MonitorObject {
    public:
        GdkMonitor* monitor;
        GdkRectangle workarea;
        GdkRectangle geometry;
        int index;
        bool primary = false;
};


class RootScreen {
    public:
        RootScreen();
        void init(Gtk::Window* window);
        virtual ~RootScreen();
        void update();
        MonitorObject* get_PrimaryMonitor();
        MonitorObject* get_NextMonitor();
        gint get_MonitorsCount();
        Struts* struts;
    private:
        MonitorObject* primaryMonitor;
        MonitorObject* nextMonitor;
        Gtk::Window* window;
        std::vector<MonitorObject*> monitors;
        GdkWindow* gdk_window;
        GtkWidget* toplevel;

        std::vector<MonitorObject*> get_Monitors();

};


#endif	/* SCREEN_H */

