
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

#include "Utilities.h"
#include <gtk/gtk.h>

#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>

#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>


namespace Utilities
{

    std::string getGtkVersion()
    {
        char buff[PATH_MAX];
        guint maxv = gtk_get_major_version();
        guint minv = gtk_get_minor_version();
        sprintf(buff, "%d.%d", maxv, minv);

        std::string result(buff);
        return result;
    }

    
    
    bool isNumeric(const std::string& s)
    {
        return( strspn( s.c_str(), " ,.0123456789" ) == s.size() );
    }
    
    /*
     * getExecPath()
     *
     * return the executable path.
     * printf( getExecPath())
     * output:
     * /sample/path
     */
    inline std::string getExecPath()
    {
        char szTmp[32];
        sprintf(szTmp, "/proc/%d/exe", getpid());
        
        char buffer[ PATH_MAX ];
        ssize_t count = readlink( szTmp, buffer, PATH_MAX );
        std::string result = std::string( buffer, (count > 0) ? count : 0 );
        std::size_t found = result.find_last_of("/");
        if (found != std::string::npos)
             result = result.substr(0, found);
        
        return result;
        
        
        
        
        
//        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
//        if (count == -1) {
//            g_critical("getExecPath Fail!\n"); 
//            return "";
//        }      
//        
//        std::string path = result;
//        std::size_t found = path.find_last_of("/\\");
//        
//        if(  found )
//            path = path.substr(0, found);
//       
//        return std::string(path);
    }

    /*
     * getExecPath(const std::string str)
     *
     * return the executable path and concatenate the given string.
     * printf( getExecPath("test.txt"))
     * output:
     * /sample/path/test.txt
     */
    std::string getExecPath(const std::string& str)
    {
        std::string path = getExecPath();
        std::string result = path + "/" + str;
        return result;
    }

    /*
     * std::vector<std::string> 
     * split(const std::string &text, char sep)
     *
     * return the tokens by delimeter separator parameter.
     * split("this_is_a_test,"_"))
     * output:
     * this
     * is
     * a
     * test
     */
    std::vector<std::string> split(const std::string &text, char sep)
    {
        std::vector<std::string> tokens;
        std::size_t start = 0, end = 0;
        while ((end = text.find(sep, start)) != std::string::npos) {
            tokens.push_back(text.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(text.substr(start));
        return tokens;
    }

    /*
     * RoundedRectangle helper
     */
    void RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
            double x, double y, double width, double height, double radius)
    {
        // radius can be no larger than half our height or width
        radius = std::min(radius, std::min(width / 2, height / 2));
        cr->move_to(x + radius, y);
        cr->arc(x + width - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
        cr->arc(x + width - radius, y + height - radius, radius, 0, M_PI * .5);
        cr->arc(x + radius, y + height - radius, radius, M_PI * .5, M_PI);
        cr->arc(x + radius, y + radius, radius, M_PI, M_PI * 1.5);

    }

    std::string removeExtension(std::string text, const char* extension)
    {
        // find extension
        std::size_t found = text.find(extension);
        if (found != std::string::npos) {
            // let's replace the extension with an empty string:
            text.replace(text.find(extension),
                    text.length(), "");
        }
        return text;
    }

    std::string removeExtension(std::string text, const std::string extensions[])
    {
        // find extensions
        for (int i = 0; i < (int) extensions->size() - 1; i++) {
            std::string e = extensions[i];
            std::size_t found = text.find(e);
            if (found != std::string::npos) {
                // let's replace the extension with an empty string:
                text.replace(text.find(e),
                        text.length(), "");
            }
        }
        return text;
    }

    /*
     * 
     * stringToLower helper.
     * 
     */
    std::string stringToLower(const char* strp)
    {
        std::string str = strp;
        transform(str.begin(), str.end(), str.begin(), ::tolower);

        return str;
    }

    std::string exec(const char* cmd)
    {
        char buffer[NAME_MAX];
        std::string result = "";
        FILE* pipe = popen(cmd, "r");
        if (!pipe) throw std::runtime_error("popen() failed!");
        try {
            while (!feof(pipe)) {
                if (fgets(buffer, 128, pipe) != NULL)
                    result += buffer;
            }
        } catch (...) {
            pclose(pipe);
            throw;
        }
        pclose(pipe);
        return result;
    }

    int system(const char* cmd)
    {
        char command[NAME_MAX];
        sprintf(command, "%s &", cmd);
        return std::system(command);
    }

    bool getMousePosition(int&x, int &y)
    {
        Window window_returned;
        int root_x, root_y;
        int win_x, win_y;
        unsigned int mask_return;

        Display *display = XOpenDisplay(NULL);
        if (display == NULL)
            return false;

        Window root_window = XRootWindow(display, XDefaultScreen(display));
        Bool found = XQueryPointer(display, root_window, &window_returned,
                &window_returned, &root_x, &root_y, &win_x, &win_y,
                &mask_return);
        if (found) {
            x = win_x;
            y = win_y;
        }

        XCloseDisplay(display);
        return found;
    }
}