//  Copyright (c) 2018-2024 Juan R. González
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <X11/Xlib.h>
#include <assert.h>
#include <dirent.h>
#include <gdk/gdkx.h>
#include <limits.h>
#include <malloc.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <filesystem>
#include "system.h"
// clang-format on
/*

 https://stackoverflow.com/questions/15000945/drawing-on-the-gtkdrawingarea-with-xlib-func



#include <X11/Xlib.h> //-lX11
#include <gdk/gdkx.h>
 #include <gtk/gtk.h> //$$(pkg-config --cflags --libs gtk+-3.0)

void DrawOnWidget(GtkWidget *widget, int width, int height)
{
    GdkDisplay *gdk_dis = gdk_display_get_default();
    Display *dis = gdk_x11_display_get_xdisplay (gdk_dis);
    GC gc = DefaultGC(dis, DefaultScreen(dis));

    GdkWindow *gdk_window = gtk_widget_get_window(widget);
    Window win = gdk_x11_window_get_xid(gdk_window);

    GdkPixbuf *pb = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, width, height);
    //GdkPixbuf *pb = gtk_image_get_pixbuf((GtkImage *) widget);
    char *data = (char *) gdk_pixbuf_read_pixels((const GdkPixbuf *) pb); //RGB(A)
    //int width = gdk_pixbuf_get_width(pb);
    //int height = gdk_pixbuf_get_height(pb);
    int pb_depth = gdk_pixbuf_get_n_channels(pb);

    int depth  = DefaultDepth(dis, DefaultScreen(dis)) / 8;
    Pixmap pm = XCreatePixmap(dis, win, width, height, depth * 8);

    unsigned long valuemask = GCForeground;
    XGCValues vColor;
    vColor.foreground = 0x00FF0000;
    XChangeGC(dis, gc, valuemask, &vColor);
    XFillRectangle(dis, pm, gc, 0, 0, width, height);
    XFlush(dis);

    XImage *ximage = XGetImage(dis, pm, 0, 0, width, height, AllPlanes, ZPixmap); //BGRX
    for(int i=0, j=0; i<width*height*pb_depth; i+=pb_depth, j+=4)
    {
        data[i+0] = ximage->data[j+2];
        data[i+1] = ximage->data[j+1];
        data[i+2] = ximage->data[j+0];
        if(pb_depth == 4) data[i+3] = 255;
    }

//    gtk_image_set_from_pixbuf((GtkImage *) widget, pb);
    XFreePixmap(dis, pm);
    g_object_unref(pb);
    return;
}
static void destroy( GtkWidget *widget, gpointer data )
{
    gtk_main_quit();
}
static void test( GtkWidget *widget, gpointer data )
{
    DrawOnWidget((GtkWidget *) data, 200, 200);
}
void GTK_Win()
{
    GtkWidget *window, *grid;
    GtkWidget *button_Exit, *button_Test;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect (window, "destroy", G_CALLBACK (destroy), NULL);

    grid = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (window), grid);

    button_Exit = gtk_button_new_with_label ("x");
    g_signal_connect (button_Exit, "clicked", G_CALLBACK (destroy), NULL);

    //GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, 200, 200);
    //GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    GtkWidget *image = gtk_image_new();

    button_Test = gtk_button_new_with_label ("Test");
    g_signal_connect (button_Test, "clicked", G_CALLBACK (test), image);
    //gtk_button_set_image((GtkButton *) button_Test, image);

    gtk_grid_attach (GTK_GRID (grid), button_Exit, 0, 0, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), button_Test, 0, 1, 1, 1);
    gtk_grid_attach (GTK_GRID (grid), image, 0, 2, 1, 1);

    gtk_widget_show_all (window);
    gtk_main();
}
int main(int argc, char** argv)
{
    gtk_init (&argc, &argv);
    GTK_Win();
    return 0;
}
 */
namespace docklight
{
    namespace system
    {

        /*
         https://stackoverflow.com/questions/19912851/get-pixmap-out-of-xlib
    XCompositeQueryExtension(dsp, &base, &br);
        XDamageQueryExtension(dsp,&base, &br);
nt main(int argc, char ** argv) {
    int br, base;
    Window win = 0x3400003;
    Display *dsp = XOpenDisplay(0);
    XCompositeQueryExtension(dsp, &base, &br);
    XDamageQueryExtension(dsp,&base, &br);

        */

        //#include <X11/Xlib.h>  //-lX11
        //#include <gdk/gdkx.h>
        //#include <gtk/gtk.h>  //$$(pkg-config --cflags --libs gtk+-3.0)
        void XDrawOnWidget(GtkWidget* widget)
        {
            GdkDisplay* gdk_dis = gdk_display_get_default();
            Display* dis = gdk_x11_display_get_xdisplay(gdk_dis);
            GC gc = DefaultGC(dis, DefaultScreen(dis));
            GdkWindow* gdk_window = gtk_widget_get_window(widget);
            Window win = gdk_x11_window_get_xid(gdk_window);
            unsigned long valuemask = GCForeground;
            XGCValues vColor;
            vColor.foreground = 0x000000FF;
            XChangeGC(dis, gc, valuemask, &vColor);
            XFillRectangle(dis, win, gc, 0, 0, 100, 100);
            XFlush(dis);
        }

        //         GtkWidget* widget = GTK_WIDGET(this->gobj());
        // GtkWidget* w = gtk_window_get_default_widget(*this->gobj());
        /*

There's a little dance with XCreateImage, XCreatePixmap and XCopyArea you have to do. It goes a
little like this:

struct Image img = get_pixels_and_geometry_from_libpng("filename.png");
XImage *img = XCreateImage(5000 paremeters);
Pixmap pixmap = XCreatePixmap(dpy, img.width, img.height, 24);
XPutImage(dpy, pixmap, gc, 0, 0, img.width, img.height);
XCopyArea(dpy, pixmap, window, 0, 0, img.width, img.height, x, y);


Well, you would probably load the XImage in with GdkPixbuf, but I just remembered something else.
You can use gdk_get_default_root_window in combination with gdk_pixbuf_get_from_window to capture
from the root window, I think that still works with GTK3 (but not GTK4). For other windows you will
likely still have to use XQueryTree from Xlib to find them, and then you can use
gdk_x11_window_foreign_new_for_display to gdk_pixbuf_get_from_window get a pixmap.



GtkWidget *widget = ...;
Window w = gdk_x11_drawable_get_xid(gtk_widget_get_window(widget));
         */

        void xdraw_window_image2(gulong xid)
        {
            if (!is_composite()) return;

            Display* dpy = XOpenDisplay(NULL);
            if (!dpy) return;

            for (int i = 0; i < ScreenCount(dpy); i++) {
                // https://linux.die.net/man/3/xcompositequeryextension
                // Note that if you know you won't be dealing with more than one window, it's better
                // to just redirect the window you're interested in using
                // XCompositeRedirectWindow(). You can then unredirect it when you're done (using
                // XCompositeUnredirectWindow()).
                XCompositeRedirectSubwindows(dpy, RootWindow(dpy, i), CompositeRedirectAutomatic);
            }

            XWindowAttributes attr;
            // XGetWindowAttributes(dpy, wId, &attr);
            //// XRenderPictFormat* format = XRenderFindVisualFormat(dpy, attr.visual);
            // auto format = XRenderFindVisualFormat(dpy, attr.visual);

            XID wId = xid;  // 0x3c0006f;
            XGetWindowAttributes(dpy, wId, &attr);

            auto format = ::XRenderFindVisualFormat(dpy, attr.visual);
            auto hasAlpha = (format->type == PictTypeDirect && format->direct.alphaMask);
            auto x = attr.x;
            auto y = attr.y;
            auto width = attr.width;
            auto height = attr.height;
            g_print("----------| x %d y %d w %d h %d\n", x, y, width, height);

            XRenderPictureAttributes pa;
            pa.subwindow_mode = IncludeInferiors;  // Don't clip child widgets
            Picture picture = XRenderCreatePicture(dpy, wId, format, CPSubwindowMode, &pa);

            int depth = DefaultDepth(dpy, DefaultScreen(dpy)) / 8;
            Pixmap pixmap = XCreatePixmap(dpy, xid, width, height, depth * 8);

            // XWindowAttributes attr;
            // XRenderPictFormat* format;
            // cairo_surface_t* surface;
            // XGetWindowAttributes(dpy, xid, &attr);

            //  format = XRenderFindVisualFormat(dpy, attr.visual);

            // surface = cairo_xlib_surface_create_with_xrender_format(
            // dpy, pixmap, attr.screen, format, attr.width, attr.height);

            /*

            7.1 Composite

            XRenderComposite exposes the RenderComposite protocol request directly.
            If a format with alpha is used, make sure it is premultiplied into the
            color channels.

                    void
                    XRenderComposite (Display   *dpy,
                                      int	    op,
                                      Picture   src,
                                      Picture   mask,
                                      Picture   dst,
                                      int	    src_x,
                                      int	    src_y,
                                      int	    mask_x,
                                      int	    mask_y,
                                      int	    dst_x,
                                      int	    dst_y,
                                      unsigned int	width,
                                      unsigned int	height)



              */
            //           pixmap = QPixmap(desk.width(), desk.height());
            XRenderComposite(dpy, hasAlpha ? PictOpOver : PictOpSrc, picture, None,
                             /*pixmap.x11PictureHandle()*/ pixmap, 0, 0, 0, 0, x, y, width, height);

            XCloseDisplay(dpy);
        }

        void xdraw_window_image(GtkWidget* widget, gulong xid, int width, int height)
        {
            //
            // void DrawOnWidget(GtkWidget *widget, int width, int height)
            //{
            GdkDisplay* gdk_dis = gdk_display_get_default();
            Display* dis = gdk_x11_display_get_xdisplay(gdk_dis);

            GC gc = DefaultGC(dis, DefaultScreen(dis));

            //  GdkWindow* gdk_window = gtk_widget_get_window(widget);
            Window win = xid;  // gdk_x11_window_get_xid(gdk_window);

            GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, width, height);
            // GdkPixbuf *pb = gtk_image_get_pixbuf((GtkImage *) widget);
            char* data = (char*)gdk_pixbuf_read_pixels((const GdkPixbuf*)pixbuf);  // RGB(A)
            // int width = gdk_pixbuf_get_width(pb);
            // int height = gdk_pixbuf_get_height(pb);
            int pb_depth = gdk_pixbuf_get_n_channels(pixbuf);

            int depth = DefaultDepth(dis, DefaultScreen(dis)) / 8;
            Pixmap pixmap = XCreatePixmap(dis, win, width, height, depth * 8);

            unsigned long valuemask = GCForeground;
            XGCValues vColor;
            vColor.foreground = 0x00FF0000;
            XChangeGC(dis, gc, valuemask, &vColor);
            XFillRectangle(dis, pixmap, gc, 0, 0, width, height);
            XFlush(dis);

            XImage* ximage =
                XGetImage(dis, pixmap, 0, 0, width, height, AllPlanes, ZPixmap);  // BGRX

            for (int i = 0, j = 0; i < width * height * pb_depth; i += pb_depth, j += 4) {
                data[i + 0] = ximage->data[j + 2];
                data[i + 1] = ximage->data[j + 1];
                data[i + 2] = ximage->data[j + 0];
                if (pb_depth == 4) data[i + 3] = 255;
            }
            // void
            // gtk_image_set_from_pixbuf (
            // GtkImage* image,
            // GdkPixbuf* pixbuf
            //)
            gtk_image_set_from_pixbuf((GtkImage*)widget, pixbuf);
            XFreePixmap(dis, pixmap);
            g_object_unref(pixbuf);
        }

        void xget_render_picture_attributes(XID wId)
        {
            Display* dpy = XOpenDisplay(NULL);
            if (!dpy) return;

            XWindowAttributes attr;
            XGetWindowAttributes(dpy, wId, &attr);
            // XRenderPictFormat* format = XRenderFindVisualFormat(dpy, attr.visual);
            auto format = XRenderFindVisualFormat(dpy, attr.visual);

            // Create a Render picture so we can reference the window contents.
            // We need to set the subwindow mode to IncludeInferiors, otherwise child widgets
            // in the window won't be included when we draw it, which is not what we want.
            XRenderPictureAttributes pa;
            pa.subwindow_mode = IncludeInferiors;  // Don't clip child widgets

            Picture picture = XRenderCreatePicture(dpy, wId, format, CPSubwindowMode, &pa);
            // TODO continue here
            // Handling shaped windows correctly
            // A windows backing pixmap is always rectangular, but if the window has a
            // non-rectangular shape we don't want to end up copying pixels that aren't a part of
            // the window when we draw it (those pixels are undefined). To avoid doing that we'll
            // set the clip region for the picture to the windows shape region. Doing this requires
            // using the XFixes extension, which you'll need to query for, just like you did with
            // composite.

            // Create a copy of the bounding region for the window
            XserverRegion region = XFixesCreateRegionFromWindow(dpy, wId, WindowRegionBounding);

            // The region is relative to the screen, not the window, so we need to offset
            // it with the windows position
            int x = 100;
            int y = 100;

            XFixesTranslateRegion(dpy, region, -x, -y);
            XFixesSetPictureClipRegion(dpy, picture, 0, 0, region);
            XFixesDestroyRegion(dpy, region);

            // It might be useful to know that when you draw a pixmap with Xrender, you provide
            // pictures both for the source and the destination drawables, and both the source and
            // destination pictures can have clip regions set. So if you're not planning on scaling
            // the window when you draw it, you could set the clip region in the destination picture
            // instead.

            // Don't forget that since the region is only a copy, you'll need to update it when the
            // window is resized or when the window shape is changed.

            // The XShape extension can provide notifications when the window shape changes, but it
            // will only do this if you explicitly ask it to. Telling XShape to send such
            // notifications is done by calling XShapeSelectInput():

            /////// NOT USE WE NEED TO SCALE      XShapeSelectInput(dpy, wId, ShapeNotifyMask);
            // Drawing the window on a QWidget or QPixmap
            // We now have all the information we need in order to be able to draw the window using
            // the Xrender extension, and we've created and prepared a source picture for the window
            // for this purpose.
            // The Xrender function we'll use to draw the window is XRenderComposite(), which is
            // defined like this in the Xrender header file:
            /*
            void XRenderComposite (Display   *dpy,
                                   int       op,
                                   Picture   src,
                                   Picture   mask,
                                   Picture   dst,
                                   int       src_x,
                                   int       src_y,
                                   int       mask_x,
                                   int       mask_y,
                                   int       dst_x,
                                   int       dst_y,
                                   unsigned int  width,
                                   unsigned int  height);
              */
            // As you can see this function takes a source picture, a destination picture and an
            // optional mask picture. In our case we have need for a mask, but we will need a
            // destination picture however.

            // We want to draw the window on a QWidget or a QPixmap, and it turns out that
            // objects of both these types already have render pictures (if Qt was built with
            // Xft/Xrender support). The picture is accessed by calling the x11RenderHandle()
            // method in the QWidget or QPixmap.

            // Another important parameter for XRenderComposite() is the second, op, which
            // specifies how the source and destination pixels should be combined. For our
            // purposes there are only two render operations that are of interest -
            // PictOpSrc and PictOpOver.

            // PictOpSrc specifies that the destination pixels should be replaced with
            // the source pixels (dst = src), including the alpha values. PictOpOver
            // corresponds to the Porter/Duff Over operator, which specifies that
            // Xrender should use the alpha values in the source pixels to blend them
            // with the destination pixels (dst = src Over dst).

            // So PictOpSrc won't blend the window, while PictOpOver will. PictOpSrc
            // is faster, and is almost always guaranteed to be accelerated, so when
            // the window doesn't have an alpha channel we'll want to use that. When
            // it has an alpha channel we'll want to use PictOpOver.

            // In the following example dest must be a QWidget or a QPixmap.
            // destX and destY are the X and Y coordinates in the widget or
            // pixmap where you want the window to be drawn.

            // [Fill the destination widget/pixmap with whatever you want to
            // use as a background here]
            /*
            XRenderComposite(dpy, hasAlpha ? PictOpOver : PictOpSrc,
            picture, None, dest.x11RenderHandle(), 0, 0, 0, 0, destX,
            destY, width, height);
*/
            XCloseDisplay(dpy);
        }

        Gdk::Rectangle xget_window_attributes(XID wId, bool& has_alpha)
        {
            Display* dpy = XOpenDisplay(NULL);
            if (!dpy) return Gdk::Rectangle(0, 0, 0, 0);

            // We need to find out some things about the window, such as its size, its position
            // on the screen, and the format of the pixel data
            XWindowAttributes attr;

            XGetWindowAttributes(dpy, wId, &attr);

            // XRenderPictFormat* format = XRenderFindVisualFormat(dpy, attr.visual);
            auto format = XRenderFindVisualFormat(dpy, attr.visual);
            bool hasAlpha = (format->type == PictTypeDirect && format->direct.alphaMask);
            int x = attr.x;
            int y = attr.y;
            int width = attr.width;
            int height = attr.height;
            has_alpha = hasAlpha;

            XCloseDisplay(dpy);

            return Gdk::Rectangle(x, y, width, height);
        }

        // https://www.talisman.org/~erlkonig/misc/x11-composite-tutorial/
        // Redirecting all toplevel windows to offscreen pixmaps
        //
        // Once you've made sure the X server supports the Composite extension, you need to make
        // sure that the contents of the window you want to grab are available in a backing pixmap.
        // This is accomplished by calling XCompositeRedirectSubwindows() once for each root window,
        // specifying that you want automatic redirection:
        //
        // This will redirect all current and future toplevel windows on each screen to offscreen
        // storage. There will only be more than one screen on a multihead (but not on a Xinerama)
        // system.
        //
        void redirecting_all_toplevel_windows_to_offscreen_pixmaps()
        {
            if (!is_composite()) return;

            Display* dpy = XOpenDisplay(NULL);
            if (!dpy) return;

            for (int i = 0; i < ScreenCount(dpy); i++) {
                // https://linux.die.net/man/3/xcompositequeryextension
                // Note that if you know you won't be dealing with more than one window, it's better
                // to just redirect the window you're interested in using
                // XCompositeRedirectWindow(). You can then unredirect it when you're done (using
                // XCompositeUnredirectWindow()).
                XCompositeRedirectSubwindows(dpy, RootWindow(dpy, i), CompositeRedirectAutomatic);
            }

            XCloseDisplay(dpy);
        }

        bool is_composite()
        {
            Display* display = XOpenDisplay(NULL);
            if (display == NULL) return false;
            //
            bool hasNamePixmap = false;
            int event_base, error_base;

            if (XCompositeQueryExtension(display, &event_base, &error_base)) {
                // If we get here the server supports the extension

                int major = 0, minor = 2;  // The highest version we support
                XCompositeQueryVersion(display, &major, &minor);

                // major and minor will now contain the highest version the server supports.
                // The protocol specifies that the returned version will never be higher
                // then the one requested. Version 0.2 is the first version to have the
                // XCompositeNameWindowPixmap() request.
                if (major > 0 || minor >= 2) hasNamePixmap = true;
            }

            XCloseDisplay(display);
            return hasNamePixmap;
        }
        bool get_mouse_position(int& x, int& y)
        {
            Window window_returned;
            int root_x, root_y;
            int win_x, win_y;
            unsigned int mask_return;

            Display* display = XOpenDisplay(NULL);
            if (display == NULL) return false;

            Window root_window = XRootWindow(display, XDefaultScreen(display));
            Bool found = XQueryPointer(display, root_window, &window_returned, &window_returned,
                                       &root_x, &root_y, &win_x, &win_y, &mask_return);
            if (found) {
                x = win_x;
                y = win_y;
            }

            XCloseDisplay(display);
            return found;
        }

        Glib::ustring get_window_manager_name()
        {
            GdkScreen* screen = gdk_screen_get_default();
            const char* wm = gdk_x11_screen_get_window_manager_name(screen);

            return wm;
        }

        bool is_mutter_window_manager()
        {
            GdkScreen* screen = gdk_screen_get_default();
            const char* wm = gdk_x11_screen_get_window_manager_name(screen);
            if (wm && (strncmp(wm, "Mutter", 5) == 0 || strncmp(wm, "GNOME", 5) == 0 ||
                       strncmp(wm, "Muffin", 5) == 0)) {
                return true;
            }

            return false;
        }

        Glib::ustring get_current_user()
        {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);
            if (pw) {
                return std::string(pw->pw_name);
            }
            return {};
        }

        bool file_exists(std::string name)
        {
            struct stat buffer;
            return (stat(name.c_str(), &buffer) == 0);
        }
        // ignore case
        std::string file_exists(const std::string& directory, std::string& file_name)
        {
            DIR* dir = nullptr;
            dir = opendir(directory.c_str());
            if (dir == 0) {
                return {};
            }
            std::string result = {};

            struct dirent* hFile;
            while ((hFile = readdir(dir)) != nullptr) {
                if (!strcmp(hFile->d_name, ".")) continue;
                if (!strcmp(hFile->d_name, "..")) continue;

                if (strcasecmp(hFile->d_name, file_name.c_str()) == 0) {
                    result = directory + hFile->d_name;
                    break;
                }
            }

            closedir(dir);
            return result;
        }

        bool is_directory_exists(const char* directory_name)
        {
            if (directory_name == nullptr) return false;

            DIR* dir;
            bool exists = false;

            dir = opendir(directory_name);

            if (dir != nullptr) {
                exists = true;
                (void)closedir(dir);
            }

            return exists;
        }

        bool create_directory(const char* directory_name)
        {
            const int dir_err = mkdir(directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            if (dir_err == -1) {
                return false;
            }

            return true;
        }

        bool create_directory_if_not_exitst(const char* directory_name)
        {
            if (!is_directory_exists(directory_name)) {
                return create_directory(directory_name);
            }

            return false;
        }

        Glib::ustring get_current_path()
        {
            char szTmp[32];
            sprintf(szTmp, "/proc/%d/exe", getpid());

            char buffer[PATH_MAX];
            ssize_t count = readlink(szTmp, buffer, PATH_MAX);
            Glib::ustring result = Glib::ustring(buffer, (count > 0) ? count : 0);
            size_t found = result.find_last_of("/");
            if (found != Glib::ustring::npos) result = result.substr(0, found);

            return result;
        }
    }  // namespace system
}  // namespace docklight
