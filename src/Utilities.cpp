
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
#include <sys/stat.h>
#include "Configuration.h"
#include  <glibmm/i18n.h>
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "DockWindow.h"

namespace Utilities
{
    namespace X11
    {
        bool is_MutterWindowManager() {

            GdkScreen* screen = gdk_screen_get_default();
            const char* wm = gdk_x11_screen_get_window_manager_name(screen);
            if (wm && strncmp(wm,"Mutter", 5) == 0) {
                return true;
            }

            return false;
        }
    }

    namespace Pixbuf
    {

        // https://developer.gnome.org/gdk3/stable/gdk3-X-Window-System-Interaction.html#gdk-x11-screen-get-number-of-desktops
        // -----------------------------------------------------------------------------------------------------------------------------------------
        // Wraps a native window in a GdkWindow. The function will try to look up the window using gdk_x11_window_lookup_for_display() first.
        // If it does not find it there, it will create a new window.
        // This may fail if the window has been destroyed. If the window was already known to GDK, a new reference to the existing GdkWindow is returned.
        // Returns a GdkWindow wrapper for the native window, or NULL if the window has been destroyed. The wrapper will be newly created, if one doesn’t exist already.
        Glib::RefPtr<Gdk::Pixbuf> get_pixbufFromWindow(const int xid)
        {
            Glib::RefPtr<Gdk::Pixbuf> result_pixbuf = NULLPB;

            GdkDisplay *gdk_display = gdk_display_get_default();
            if(gdk_display == nullptr){
                return NULLPB;
            }

            GdkWindow *gdk_window = gdk_x11_window_foreign_new_for_display(gdk_display, xid);
            if(gdk_window == nullptr){
                return NULLPB;
            }

            // Gets the window size
            guint winWidth = gdk_window_get_width(gdk_window);
            guint winHeight = gdk_window_get_height(gdk_window);

            // This function will create an RGB pixbuf with 8 bits per channel with the size specified by the width and height
            // arguments scaled by the scale factor of window . The pixbuf will contain an alpha channel if the window contains one.
            // If the window is off the screen, then there is no image data in the obscured/offscreen regions to be placed in the pixbuf.
            // The contents of portions of the pixbuf corresponding to the offscreen region are undefined.
            // ---------------------------------------------------------------------------------------------------------------------------
            // If the window you’re obtaining data from is partially obscured by other windows, then the contents of the pixbuf areas
            // corresponding to the obscured regions are undefined.
            // ---------------------------------------------------------------------------------------------------------------------------
            // If the window is not mapped (typically because it’s iconified/minimized or not on the current workspace), then NULL will be returned.
            // If memory can’t be allocated for the return value, NULL will be returned instead.
            // (In short, there are several ways this function can fail, and if it fails it returns NULL; so check the return value.)
            // ---------------------------------------------------------------------------------------------------------------------------
            // creates a newly pixbuf with a reference count of 1, or NULL on error.

            GdkPixbuf *winPixbuf = gdk_pixbuf_get_from_window(gdk_window, 0, 0, winWidth, winHeight);
            if (winPixbuf == nullptr) {
                return NULLPB;
            }

            result_pixbuf = Glib::wrap(winPixbuf, true);
            g_object_unref(winPixbuf);

            return result_pixbuf;
        }


        Glib::RefPtr<Gdk::Pixbuf> get_pixbufScaled(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf, const guint destWidth, const guint destHeight, guint& scaledWidth, guint& scaledHeight)
        {
            if (!pixbuf) {
                return NULLPB;
            }

            // sets the source size
            guint winWidth = pixbuf->get_width();
            guint winHeight = pixbuf->get_height();

            // sets the target size
            guint width = destWidth;
            guint height = destHeight;

            // calculate aspect ratio
            double minSize = std::min(width, height);
            double maxSize = std::max(winWidth, winHeight);
            double aspectRatio = minSize / maxSize;

            scaledWidth = abs(winWidth * aspectRatio);
            scaledHeight = abs(winHeight * aspectRatio);

            guint half_WindowWidth = DockWindow::Monitor::get_workarea().width / 2 ;
            guint half_WindowHeight = DockWindow::Monitor::get_workarea().height / 2;

            // ajust width size to make it looks better
            if(winWidth > half_WindowWidth ) {
                if(winHeight > half_WindowHeight )
                    scaledWidth = width - 4;
            }

            // ajust height size to make it looks better
            if(winHeight > half_WindowHeight ) {
                if(winWidth > half_WindowWidth )
                    scaledHeight = height- 2;
            }

            return pixbuf->scale_simple(scaledWidth, scaledHeight, Gdk::INTERP_BILINEAR);
        }

        /**
         * Compares the given pixbuf data.
         *
         * @params  const GdkPixbuf* pixbuf
         * @params  const guint8* pixels_first
         * @params  const guint8* pixels_current
         *
         * @return 1 if the pixbuf data contains diferences, or 0 if the pixels are equal otherwise -1.
         */
        int ComparePixels(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_a, const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_b)
        {
            if(!pixbuf_b || !pixbuf_b){
                return -1;
            }

            if (pixbuf_a->get_bits_per_sample() != 8 || pixbuf_a->get_colorspace() != Gdk::COLORSPACE_RGB) {
                return -1;
            }

            if (pixbuf_b->get_bits_per_sample() != 8 || pixbuf_b->get_colorspace() != Gdk::COLORSPACE_RGB) {
                return -1;
            }

            int  x, y;
            int  w = pixbuf_a->get_width();
            int h = pixbuf_a->get_height();
            int channels = pixbuf_a->get_n_channels();
            gint rowstride = pixbuf_a->get_rowstride();
            gint pixel_offset = 0;

            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    pixel_offset = y * rowstride + x * channels;

                    guchar* firstPixel = &pixbuf_a->get_pixels()[pixel_offset];
                    guchar* currentPixel = &pixbuf_b->get_pixels()[pixel_offset];

                    if (firstPixel[0] != currentPixel[0] || firstPixel[1] != currentPixel[1] ||  firstPixel[2] != currentPixel[2]){
                        return 1;
                    }
                }
            }

            return 0;
        }
    }





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
        return ( strspn(s.c_str(), " ,.0123456789") == s.size());
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
        ssize_t count = readlink(szTmp, buffer, PATH_MAX);
        std::string result = std::string(buffer, (count > 0) ? count : 0);
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
        for (int i = 0; i < (int)extensions->size() - 1; i++) {
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
        }
        catch (...) {
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

    std::vector<std::string>& Arguments()
    {
        static std::vector<std::string> arguments;
        return arguments;
    }

#include <dirent.h>

    bool DirectoryExists(const char* pzPath)
    {
        if (pzPath == NULL) return false;

        DIR *pDir;
        bool bExists = false;

        pDir = opendir(pzPath);

        if (pDir != NULL) {
            bExists = true;
            (void)closedir(pDir);
        }

        return bExists;
    }


    bool CreateDirectory(const char* dirNamePath)
    {
        const int dir_err = mkdir(dirNamePath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_err) {
            printf("Error creating directory!n");
            return false;
        }

        return true;
    }

    bool CreateDirectoryIfNotExitst(const char* dirNamePath)
    {
        if(!DirectoryExists(dirNamePath)){
         return CreateDirectory(dirNamePath);
        }
        return false;
    }

    namespace Messages
    {
        // https://unix.stackexchange.com/questions/144924/how-to-create-a-message-box-from-the-command-line
        void NotifySend(const std::string& title, const std::string& message) {
            std::stringstream command;
            command << "notify-send '" << title << "' '" << message <<"' ";

            Utilities::exec(command.str().c_str());
        }


        void Zenity(const std::string& title, const std::string& message) {
            std::stringstream command;
            command << "zenity --error --text='"<< message  << "' --title='Warning!'";

            Utilities::exec(command.str().c_str());
        }


        void LimitReachedMessage()
        {
            char message[PATH_MAX];
            sprintf(message, _("There are to many windows open for display them correctly. Please close some windows and try again."));
            Utilities::Messages::NotifySend(_("Warning"), message);

        }
    }
}
