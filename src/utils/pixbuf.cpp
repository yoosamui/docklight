#include "utils/pixbuf.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
#include <glibmm/fileutils.h>
#include <gtkmm/iconinfo.h>
#include <gtkmm/icontheme.h>
#include "utils/position.h"
#include "utils/system.h"

DL_NS_BEGIN

namespace pixbuf_util
{
    void invert_pixels(Glib::RefPtr<Gdk::Pixbuf> image)
    {
        if (image->get_colorspace() != Gdk::COLORSPACE_RGB || image->get_bits_per_sample() != 8) {
            return;
        }
        gint x, y;
        int w = image->get_width();
        int h = image->get_height();
        int channels = image->get_n_channels();
        gint rowstride = image->get_rowstride();
        gint pixel_offset;

        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                pixel_offset = y * rowstride + x * channels;
                guchar* pixel = &image->get_pixels()[pixel_offset];

                pixel[0] = 255 - pixel[0];
                pixel[1] = 255 - pixel[1];
                pixel[2] = 255 - pixel[2];
            }
        }
    }

    int compare_pixels(const GdkPixbuf* pixbuf_a, const GdkPixbuf* pixbuf_b, bool validate = true)
    {
        if (!pixbuf_b || !pixbuf_b) {
            return -1;
        }

        if (validate) {
            if (gdk_pixbuf_get_bits_per_sample(pixbuf_a) != 8 ||
                gdk_pixbuf_get_colorspace(pixbuf_a) != GDK_COLORSPACE_RGB) {
                return -1;
            }

            if (gdk_pixbuf_get_bits_per_sample(pixbuf_b) != 8 ||
                gdk_pixbuf_get_colorspace(pixbuf_b) != GDK_COLORSPACE_RGB) {
                return -1;
            }
        }

        int x, y;
        int w = gdk_pixbuf_get_width(pixbuf_a);
        int h = gdk_pixbuf_get_height(pixbuf_a);
        int channels = gdk_pixbuf_get_n_channels(pixbuf_a);
        gint rowstride = gdk_pixbuf_get_rowstride(pixbuf_a);
        gint pixel_offset = 0;
        guchar* pixels_a;
        guchar* pixels_b;

        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                pixel_offset = y * rowstride + x * channels;
                pixels_a = &gdk_pixbuf_get_pixels(pixbuf_a)[pixel_offset];
                pixels_b = &gdk_pixbuf_get_pixels(pixbuf_b)[pixel_offset];

                // clang-format off
                if (pixels_a[0] != pixels_b[0] ||
                    pixels_a[1] != pixels_b[1] ||
                    pixels_a[2] != pixels_b[2] ){
                    return 0;
                }
                // clang-format on
            }
        }

        return 1;
    }

    int compare_pixels(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_a,
                       const Glib::RefPtr<Gdk::Pixbuf>& pixbuf_b, bool validate = true)
    {
        if (!pixbuf_b || !pixbuf_b) {
            return -1;
        }

        if (validate) {
            if (pixbuf_a->get_bits_per_sample() != 8 ||
                pixbuf_a->get_colorspace() != Gdk::COLORSPACE_RGB) {
                return -1;
            }

            if (pixbuf_b->get_bits_per_sample() != 8 ||
                pixbuf_b->get_colorspace() != Gdk::COLORSPACE_RGB) {
                return -1;
            }
        }

        int x, y;
        int w = pixbuf_a->get_width();
        int h = pixbuf_a->get_height();
        int channels = pixbuf_a->get_n_channels();
        gint rowstride = pixbuf_a->get_rowstride();
        gint pixel_offset = 0;

        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                pixel_offset = y * rowstride + x * channels;

                guchar* pixels_a = &pixbuf_a->get_pixels()[pixel_offset];
                guchar* pixels_b = &pixbuf_b->get_pixels()[pixel_offset];

                // clang-format off
                if (pixels_a[0] != pixels_b[0] ||
                    pixels_a[1] != pixels_b[1] ||
                    pixels_a[2] != pixels_b[2]) {
                    return 0;
                }
                // clang-format on
            }
        }

        return 1;
    }

    const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string& filename, int width,
                                                  int height)
    {
        Glib::RefPtr<Gdk::Pixbuf> result = NULLPB;
        if (system_util::file_exists(filename)) {
            try {
                result = Gdk::Pixbuf::create_from_file(filename, width, height, true);
            } catch (const Glib::FileError& ex) {
                g_critical("get_from file: %s FileError: %s", filename.c_str(), ex.what().c_str());
            } catch (const Gdk::PixbufError& ex) {
                g_critical("get_from file: %s PixbufError: %s", filename.c_str(),
                           ex.what().c_str());
            }
        }
        return result;
    }

    // const Glib::RefPtr<Gdk::Pixbuf> get_icon_from_info(const appinfo_t& info, int size)
    //{
    // Glib::RefPtr<Gdk::Pixbuf> empty = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;
    // GtkIconTheme* icon_theme = gtk_icon_theme_get_default();
    // auto pixbuf = gtk_icon_theme_load_icon(icon_theme,
    // info.m_icon_name.c_str(),     // icon name
    // size,                         // icon size
    // GTK_ICON_LOOKUP_USE_BUILTIN,  // flags //
    //&error);
    // if (error) {
    // g_error_free(error);
    // error = nullptr;
    //}
    //}

    const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(WnckWindow* window, const string& icon_name,
                                                    int size)
    {
        Glib::RefPtr<Gdk::Pixbuf> empty = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;
        if (icon_name.length() == 0) {
            return empty;
        }

        GError* error = nullptr;
        GtkIconTheme* icon_theme = gtk_icon_theme_get_default();
        auto pixbuf = gtk_icon_theme_load_icon(icon_theme,
                                               icon_name.c_str(),           // icon name
                                               size,                        // icon size
                                               GTK_ICON_LOOKUP_FORCE_SIZE,  // flags //
                                               &error);
        if (error) {
            string error_message = error->message;
            g_error_free(error);
            error = nullptr;

            if (WNCK_IS_WINDOW(window)) {
                auto icon = wnck_window_get_icon(window);
                return Glib::wrap(icon, true)->scale_simple(size, size, Gdk::INTERP_BILINEAR);
            }

            // try load from file
            auto pixbuf = get_from_file(icon_name.c_str(), size, size);
            if (pixbuf) {
                return pixbuf;
            } else {
                g_warning(
                    "Can't load icon from theme.\n Can't load icon from file \nwindow is NULL\n "
                    "Theme error: %s\n",
                    error_message.c_str());
            }

            return empty;
        }

        return Glib::wrap(pixbuf, true);
    }

    // https://developer.gnome.org/gdk3/stable/gdk3-X-Window-System-Interaction.html#gdk-x11-screen-get-number-of-desktops
    // -----------------------------------------------------------------------------------------------------------------------------------------
    // Wraps a native window in a GdkWindow. The function will try to look up the window using
    // gdk_x11_window_lookup_for_display() first. If it does not find it there, it will create a new
    // window. This may fail if the window has been destroyed. If the window was already known to
    // GDK, a new reference to the existing GdkWindow is returned. Returns a GdkWindow wrapper for
    // the native window, or NULL if the window has been destroyed. The wrapper will be newly
    // created, if one doesn’t exist already.
    //
    // https://gist.github.com/mmozeiko/2401933b1fa89e5d5bd238b33eab0465
    // https://developer.gnome.org/gdk-pixbuf/stable/gdk-pixbuf-File-saving.html#gdk-pixbuf-save
    // https://mail.gnome.org/archives/gtk-list/2004-October/msg00186.html
    const Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_from_window(int xid)
    {
        Glib::RefPtr<Gdk::Pixbuf> result_pixbuf = NULLPB;

        GdkDisplay* gdk_display = gdk_display_get_default();
        if (gdk_display == nullptr) {
            return NULLPB;
        }

        GdkWindow* gdk_window = gdk_x11_window_foreign_new_for_display(gdk_display, xid);
        if (gdk_window == nullptr) {
            return NULLPB;
        }

        // Gets the window size
        guint winWidth = gdk_window_get_width(gdk_window);
        guint winHeight = gdk_window_get_height(gdk_window);

        // This function will create an RGB pixbuf with 8 bits per channel with the size specified
        // by the width and height arguments scaled by the scale factor of window . The pixbuf will
        // contain an alpha channel if the window contains one. If the window is off the screen,
        // then there is no image data in the obscured/offscreen regions to be placed in the pixbuf.
        // The contents of portions of the pixbuf corresponding to the offscreen region are
        // undefined.
        // ---------------------------------------------------------------------------------------------------------------------------
        // If the window you’re obtaining data from is partially obscured by other windows, then the
        // contents of the pixbuf areas corresponding to the obscured regions are undefined.
        // ---------------------------------------------------------------------------------------------------------------------------
        // If the window is not mapped (typically because it’s iconified/minimized or not on the
        // current workspace), then NULL will be returned. If memory can’t be allocated for the
        // return value, NULL will be returned instead. (In short, there are several ways this
        // function can fail, and if it fails it returns NULL; so check the return value.)
        // ---------------------------------------------------------------------------------------------------------------------------
        // creates a newly pixbuf with a reference count of 1, or NULL on error.

        GdkPixbuf* winPixbuf = gdk_pixbuf_get_from_window(gdk_window, 0, 0, winWidth, winHeight);
        if (winPixbuf == nullptr) {
            return NULLPB;
        }

        result_pixbuf = Glib::wrap(winPixbuf, true);
        g_object_unref(winPixbuf);

        return result_pixbuf;
    }
    GdkPixbuf* get_gdk_pixbuf_from_window(int xid)
    {
        GdkPixbuf* winPixbuf = nullptr;

        GdkDisplay* gdk_display = gdk_display_get_default();
        if (gdk_display == nullptr) {
            return nullptr;
        }

        GdkWindow* gdk_window = gdk_x11_window_foreign_new_for_display(gdk_display, xid);
        if (gdk_window == nullptr) {
            return nullptr;
        }

        // Gets the window size
        guint winWidth = gdk_window_get_width(gdk_window);
        guint winHeight = gdk_window_get_height(gdk_window);

        winPixbuf = gdk_pixbuf_get_from_window(gdk_window, 0, 0, winWidth, winHeight);
        if (winPixbuf == nullptr) {
            return nullptr;
        }

        //        result_pixbuf = Glib::wrap(winPixbuf, true);
        //        g_object_unref(winPixbuf);

        return winPixbuf;
    }
    const Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_from_window(int xid, int width, int height)
    {
        Glib::RefPtr<Gdk::Pixbuf> result_pixbuf = NULLPB;

        GdkDisplay* gdk_display = gdk_display_get_default();
        if (gdk_display == nullptr) {
            return NULLPB;
        }

        GdkWindow* gdk_window = gdk_x11_window_foreign_new_for_display(gdk_display, xid);
        if (gdk_window == nullptr) {
            return NULLPB;
        }

        // Gets the window size
        guint winWidth = width;    // gdk_window_get_width(gdk_window);
        guint winHeight = height;  // gdk_window_get_height(gdk_window);

        // This function will create an RGB pixbuf with 8 bits per channel with the size specified
        // by the width and height arguments scaled by the scale factor of window . The pixbuf will
        // contain an alpha channel if the window contains one. If the window is off the screen,
        // then there is no image data in the obscured/offscreen regions to be placed in the pixbuf.
        // The contents of portions of the pixbuf corresponding to the offscreen region are
        // undefined.
        // ---------------------------------------------------------------------------------------------------------------------------
        // If the window you’re obtaining data from is partially obscured by other windows, then the
        // contents of the pixbuf areas corresponding to the obscured regions are undefined.
        // ---------------------------------------------------------------------------------------------------------------------------
        // If the window is not mapped (typically because it’s iconified/minimized or not on the
        // current workspace), then NULL will be returned. If memory can’t be allocated for the
        // return value, NULL will be returned instead. (In short, there are several ways this
        // function can fail, and if it fails it returns NULL; so check the return value.)
        // ---------------------------------------------------------------------------------------------------------------------------
        // creates a newly pixbuf with a reference count of 1, or NULL on error.

        GdkPixbuf* winPixbuf = gdk_pixbuf_get_from_window(gdk_window, 0, 0, winWidth, winHeight);
        if (winPixbuf == nullptr) {
            return NULLPB;
        }

        result_pixbuf = Glib::wrap(winPixbuf, true);
        g_object_unref(winPixbuf);

        return result_pixbuf;
    }

    GdkPixbuf* get_pixbuf_from_window_raw(int xid)
    {
        GdkDisplay* gdk_display = gdk_display_get_default();
        if (gdk_display == nullptr) {
            return nullptr;
        }

        GdkWindow* gdk_window = gdk_x11_window_foreign_new_for_display(gdk_display, xid);
        if (gdk_window == nullptr) {
            return nullptr;
        }

        // Gets the window size
        guint winWidth = gdk_window_get_width(gdk_window);
        guint winHeight = gdk_window_get_height(gdk_window);

        // This function will create an RGB pixbuf with 8 bits per channel with the size specified
        // by the width and height arguments scaled by the scale factor of window . The pixbuf will
        // contain an alpha channel if the window contains one. If the window is off the screen,
        // then there is no image data in the obscured/offscreen regions to be placed in the pixbuf.
        // The contents of portions of the pixbuf corresponding to the offscreen region are
        // undefined.
        // ---------------------------------------------------------------------------------------------------------------------------
        // If the window you’re obtaining data from is partially obscured by other windows, then the
        // contents of the pixbuf areas corresponding to the obscured regions are undefined.
        // ---------------------------------------------------------------------------------------------------------------------------
        // If the window is not mapped (typically because it’s iconified/minimized or not on the
        // current workspace), then NULL will be returned. If memory can’t be allocated for the
        // return value, NULL will be returned instead. (In short, there are several ways this
        // function can fail, and if it fails it returns NULL; so check the return value.)
        // ---------------------------------------------------------------------------------------------------------------------------
        // creates a newly pixbuf with a reference count of 1, or NULL on error.

        GdkPixbuf* winPixbuf = gdk_pixbuf_get_from_window(gdk_window, 0, 0, winWidth, winHeight);
        if (winPixbuf == nullptr) {
            return nullptr;
        }

        return winPixbuf;
    }

    /*Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_scaled(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf,
                                                const guint destWidth, const guint destHeight,
                                                guint& scaledWidth, guint& scaledHeight)
    {
        if (!pixbuf) {
            return NULLPB;
        }

        Glib::RefPtr<Gdk::Pixbuf> result =
            calculate_aspect_ratio(pixbuf, destWidth, destHeight, scaledWidth, scaledHeight);

        // guint winWidth = result->get_width();
        // guint winHeight = result->get_height();

        // double percent_w = (scaledWidth * 100) / destWidth;
        // double percent_h = (scaledHeight * 100) / destHeight;

        // if (percent_w < 90.f || percent_h < 90.f) {
        // int factor = 10;  // destWidth + percent_w / 2;
        // result = calculate_aspect_ratio(pixbuf, destWidth + factor, destHeight + factor,
        // scaledWidth, scaledHeight);
        //}

        //        g_print("des %d x %d  res %d %d  p:%f\n", destWidth, destHeight, winWidth,
        //        winHeight,
        //              percent);

        //// ajust width size to make it looks better
        // if (winWidth > half_WindowWidth) {
        // if (winHeight > half_WindowHeight) scaledWidth = width - 4;
        //}

        //// ajust height size to make it looks better
        // if (winHeight > half_WindowHeight) {
        // if (winWidth > half_WindowWidth) scaledHeight = height - 2;
        //}

        return result;  //->scale_simple(scaledWidth, scaledHeight, Gdk::INTERP_BILINEAR);
    }*/

    Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_scaled(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf,
                                                const guint destWidth, const guint destHeight,
                                                guint& scaledWidth, guint& scaledHeight)
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

        auto const workarea = position_util::get_workarea();

        guint half_WindowWidth = workarea.get_width() / 3;
        guint half_WindowHeight = workarea.get_height() / 3;

        // ajust width size
        if (winWidth > half_WindowWidth) {
            scaledWidth = width - 6;
        }

        // ajust height size
        if (winHeight > half_WindowHeight) {
            scaledHeight = height - 4;
        }

        return pixbuf->scale_simple(scaledWidth, scaledHeight, Gdk::INTERP_BILINEAR);
    }

    GdkPixbuf* get_gdk_pixbuf_scaled(const GdkPixbuf* pixbuf, const guint destWidth,
                                     const guint destHeight)
    {
        if (!pixbuf) {
            return nullptr;
        }

        // sets the source size
        guint winWidth = gdk_pixbuf_get_width(pixbuf);
        guint winHeight = gdk_pixbuf_get_height(pixbuf);

        // sets the target size
        guint width = destWidth;
        guint height = destHeight;

        // calculate aspect ratio
        double minSize = std::min(width, height);
        double maxSize = std::max(winWidth, winHeight);
        double aspectRatio = minSize / maxSize;

        int scaledWidth = abs(winWidth * aspectRatio);
        int scaledHeight = abs(winHeight * aspectRatio);

        auto const workarea = position_util::get_workarea();

        guint half_WindowWidth = workarea.get_width() / 3;
        guint half_WindowHeight = workarea.get_height() / 3;

        // ajust width size
        if (winWidth > half_WindowWidth) {
            scaledWidth = width - 6;
        }

        // ajust height size
        if (winHeight > half_WindowHeight) {
            scaledHeight = height - 4;
        }

        return gdk_pixbuf_scale_simple(pixbuf, scaledWidth, scaledHeight, GDK_INTERP_BILINEAR);
    }

    /**
     * Converts a GdkPixbuf to a Glib::RefPtr<Gdk::Pixbuf>.
     * DEPRECATED
     */
    const Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon)
    {
        Glib::RefPtr<Gdk::Pixbuf> result;

        int width = gdk_pixbuf_get_width(icon);
        int height = gdk_pixbuf_get_height(icon);
        int stride = gdk_pixbuf_get_rowstride(icon);
        gboolean hasalpha = gdk_pixbuf_get_has_alpha(icon);
        int bitsaple = gdk_pixbuf_get_bits_per_sample(icon);
        guint8* pdata;
        guchar* pixels = gdk_pixbuf_get_pixels(icon);

        pdata = static_cast<guint8*>(pixels);

        result = Gdk::Pixbuf::create_from_data(pdata, Gdk::COLORSPACE_RGB, hasalpha, bitsaple,
                                               width, height, stride);

        return result;
    }
}  // namespace pixbuf_util

DL_NS_END

