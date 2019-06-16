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
#ifndef UTILITIES_H
#define	UTILITIES_H

#include <gtk/gtk.h>
#include <gtkmm.h>


#include <string>
#include <algorithm>
#include <math.h>
#include <vector>


namespace Utilities
{

    #define CHECK_BIT(var,pos) ((var & (1 << pos)) == (1 << pos))

    std::string getExecPath();
    std::string getExecPath(const std::string& str);

    std::vector<std::string> split(const std::string &text, char sep);
    void RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
            double x, double y, double width, double height, double radius);
    std::string removeExtension(std::string text, const char* extension);
    std::string removeExtension(std::string text, const std::string extensions[]);
    std::string stringToLower(const char* strp);
    std::string getGtkVersion();
    std::string exec(const char* cmd);
    int system(const char* cmd);
    bool getMousePosition(int&x,int &y);
    bool isNumeric(const std::string& s);
    std::vector<std::string>& Arguments();
    bool DirectoryExists(const char* pzPath);
    bool CreateDirectoryIfNotExitst(const char* dirNamePath);
    bool CreateDirectory(const char* dirNamePath);

    namespace X11
    {
        bool is_MutterWindowManager();
    }

    namespace Messages
    {
        // https://unix.stackexchange.com/questions/144924/how-to-create-a-message-box-from-the-command-line
        void NotifySend(const std::string& title, const std::string& message);
        void Zenity(const std::string& title, const std::string& message);
        void LimitReachedMessage();
    }

    namespace Pixbuf
    {
        Glib::RefPtr<Gdk::Pixbuf> get_pixbufFromWindow(const int xid);
        Glib::RefPtr<Gdk::Pixbuf> get_pixbufScaled(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf, const guint destWidth, const guint destHeight, guint& scaledWidth, guint& scaledHeight);
        int ComparePixels(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_a, const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_b);
    }

}

#endif	/* UTILITIES_H */

