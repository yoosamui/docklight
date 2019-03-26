//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 12:17 PM 
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

#ifndef TITLEWINDOW_H
#define	TITLEWINDOW_H

#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>

class TitleWindow : public Gtk::Window
{
public:
    TitleWindow();
    virtual ~TitleWindow();
    void setText(const Glib::ustring text);
        
private:
    
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    
    Pango::FontDescription font;
    
    //Child widgets:
    Gtk::Box m_HBox;
    Gtk::Label m_Label;
};

#endif	/* TITLEWINDOW_H */

