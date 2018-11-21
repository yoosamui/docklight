//*****************************************************************
//
//  Copyright (C) 2016 Juan R. Gonzalez
//  Created on December, 2016 
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

#ifndef ICONLOADER_H
#define ICONLOADER_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <gdkmm/pixbuf.h>

namespace IconLoader
{
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window);
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window, std::string &theme_iconname);
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window,GdkPixbuf *icon,std::string &theme_iconname);
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIconFromDesktopFile(WnckWindow* window,GdkPixbuf *icon);
    Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon);
    Glib::RefPtr<Gdk::Pixbuf> GetIconByAppName(const char* appname,std::string &theme_iconname);
}

#endif /* ICONLOADER_H */

