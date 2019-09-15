#include "utils/pixbuf.h"
#include <glibmm/fileutils.h>
#include <gtkmm/iconinfo.h>
#include <gtkmm/icontheme.h>
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

    const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string& filename, int width,
                                                  int height)
    {
        Glib::RefPtr<Gdk::Pixbuf> result;
        if (system_util::file_exists(filename)) {
            try {
                result = Gdk::Pixbuf::create_from_file(filename, width, height, true);
            } catch (const Glib::FileError& ex) {
                g_error("get_from file: %s FileError: %s", filename.c_str(), ex.what().c_str());
            } catch (const Gdk::PixbufError& ex) {
                g_error("get_from file: %s PixbufError: %s", filename.c_str(), ex.what().c_str());
            }
        }
        return result;
    }

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

