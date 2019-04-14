//*****************************************************************
//
//  Copyright (C) 2019 g R. Gonzalez
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

#ifndef DRAGDROPWINDOW_H
#define DRAGDROPWINDOW_H

//#include "AppWindowBase.h" 

#include <gtkmm/window.h>
#include <gdkmm/pixbuf.h>
#include "DockItem.h"

class DragAndDropWindow :  public Gtk::Window
{
    public:
        DragAndDropWindow();
        void Show(const Glib::RefPtr<Gdk::Pixbuf>& icon, DockItem* item, Gdk::Point mousePoint);

    private:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
        Glib::RefPtr<Gdk::Pixbuf> m_icon;
        bool on_timeoutDraw();
        bool Move(int x, int y);
        Gdk::Point m_mousePoint;
        DockItem* m_item;
};



#endif /* DRAGDROPWINDOW_H */
