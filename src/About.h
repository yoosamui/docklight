//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on January  2017
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

#ifndef ABOUT_H
#define ABOUT_H

#include <gtkmm.h>

class About : public Gtk::AboutDialog
{
private:
    Glib::ustring m_appName, m_appVersion;
    std::vector<Glib::ustring> m_authors;
    std::vector<Glib::ustring> m_artists;
    std::string m_logoPath;
    gboolean m_isInitSet;
    void init(Window* window);
    void signal_about_dlg_response(int response_id);
    
    
public:
    About();
    void show(Gtk::Window* window);

};

#endif /* ABOUT_H */

