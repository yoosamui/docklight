#include "utils/pixbuf.h"
// #include "components/capture.h"
#include "components/capture.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>
#include <glibmm/fileutils.h>
#include <gtkmm/iconinfo.h>
#include <gtkmm/icontheme.h>

// #include "utils/position.h"
// #include "utils/system.h"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

namespace docklight
{

    namespace pixbuf
    {

        bool xcomposite_supported()
        {
            GdkDisplay *gdkDisplay = gdk_display_get_default();

            if (!gdkDisplay)
                return false;

            Display *display =
                GDK_DISPLAY_XDISPLAY(gdkDisplay);

            int eventBase;
            int errorBase;

            return XCompositeQueryExtension(
                display,
                &eventBase,
                &errorBase);
        }

        void invert_pixels(Glib::RefPtr<Gdk::Pixbuf> image)
        {
            if (image->get_colorspace() != Gdk::COLORSPACE_RGB ||
                image->get_bits_per_sample() != 8)
            {
                return;
            }
            gint x, y;
            int w = image->get_width();
            int h = image->get_height();
            int channels = image->get_n_channels();
            gint rowstride = image->get_rowstride();
            gint pixel_offset;

            for (y = 0; y < h; y++)
            {
                for (x = 0; x < w; x++)
                {
                    pixel_offset = y * rowstride + x * channels;
                    guchar *pixel = &image->get_pixels()[pixel_offset];

                    pixel[0] = 255 - pixel[0];
                    pixel[1] = 255 - pixel[1];
                    pixel[2] = 255 - pixel[2];
                }
            }
        }

        int compare_pixels(const GdkPixbuf *pixbuf_a, const GdkPixbuf *pixbuf_b,
                           bool validate = true)
        {
            if (!pixbuf_b || !pixbuf_b)
            {
                return -1;
            }

            if (validate)
            {
                if (gdk_pixbuf_get_bits_per_sample(pixbuf_a) != 8 ||
                    gdk_pixbuf_get_colorspace(pixbuf_a) != GDK_COLORSPACE_RGB)
                {
                    return -1;
                }

                if (gdk_pixbuf_get_bits_per_sample(pixbuf_b) != 8 ||
                    gdk_pixbuf_get_colorspace(pixbuf_b) != GDK_COLORSPACE_RGB)
                {
                    return -1;
                }
            }

            int x, y;
            int w = gdk_pixbuf_get_width(pixbuf_a);
            int h = gdk_pixbuf_get_height(pixbuf_a);
            int channels = gdk_pixbuf_get_n_channels(pixbuf_a);
            gint rowstride = gdk_pixbuf_get_rowstride(pixbuf_a);
            gint pixel_offset = 0;
            guchar *pixels_a;
            guchar *pixels_b;

            for (y = 0; y < h; y++)
            {
                for (x = 0; x < w; x++)
                {
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

        int compare_pixels(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf_a,
                           const Glib::RefPtr<Gdk::Pixbuf> &pixbuf_b, bool validate = true)
        {
            if (!pixbuf_b || !pixbuf_b)
            {
                return -1;
            }

            if (validate)
            {
                if (pixbuf_a->get_bits_per_sample() != 8 ||
                    pixbuf_a->get_colorspace() != Gdk::COLORSPACE_RGB)
                {
                    return -1;
                }

                if (pixbuf_b->get_bits_per_sample() != 8 ||
                    pixbuf_b->get_colorspace() != Gdk::COLORSPACE_RGB)
                {
                    return -1;
                }
            }

            int x, y;
            int w = pixbuf_a->get_width();
            int h = pixbuf_a->get_height();
            int channels = pixbuf_a->get_n_channels();
            gint rowstride = pixbuf_a->get_rowstride();
            gint pixel_offset = 0;

            for (y = 0; y < h; y++)
            {
                for (x = 0; x < w; x++)
                {
                    pixel_offset = y * rowstride + x * channels;

                    guchar *pixels_a = &pixbuf_a->get_pixels()[pixel_offset];
                    guchar *pixels_b = &pixbuf_b->get_pixels()[pixel_offset];

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

        const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string &filename, int width,
                                                      int height)
        {
            Glib::RefPtr<Gdk::Pixbuf> result;
            // if (system::file_exists(filename)) {
            // try {
            // result = Gdk::Pixbuf::create_from_file(filename, width, height, true);
            //} catch (const Glib::FileError& ex) {
            // g_critical("get_from file: %s FileError: %s", filename.c_str(), ex.what().c_str());
            //} catch (const Gdk::PixbufError& ex) {
            // g_critical("get_from file: %s PixbufError: %s", filename.c_str(),
            // ex.what().c_str());
            //}
            //}
            return result;
        }

        const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(WnckWindow *window,
                                                        const std::string &icon_name, int size)
        {
            Glib::RefPtr<Gdk::Pixbuf> empty = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;
            if (icon_name.length() == 0)
            {
                return empty;
            }

            //    auto pixbuf = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;
            //      string error_message = "ERROR";

            GError *error = nullptr;
            GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
            auto pixbuf = gtk_icon_theme_load_icon(icon_theme,
                                                   icon_name.c_str(),          // icon name
                                                   size,                       // icon size
                                                   GTK_ICON_LOOKUP_FORCE_SIZE, // flags //
                                                   &error);

            if (error)
            {
                std::string error_message = error->message;
                g_error_free(error);
                error = nullptr;

                if (WNCK_IS_WINDOW(window))
                {
                    auto icon = wnck_window_get_icon(window);
                    return Glib::wrap(icon, true)->scale_simple(size, size, Gdk::INTERP_BILINEAR);
                }

                // try load from file
                auto pixbuf = get_from_file(icon_name.c_str(), size, size);
                if (pixbuf)
                {
                    return pixbuf;
                }
                else
                {
                    g_warning(
                        "Can't load icon from theme.\n Can't load icon from file \nwindow is "
                        "NULL\n "
                        "Theme error: %s\n",
                        error_message.c_str());
                }

                return empty;
            }

            return Glib::wrap(pixbuf, true);
        }

        bool get_window_image(gulong xid,
                              Glib::RefPtr<Gdk::Pixbuf> &image,
                              guint size)
        {

            GdkPixbuf *raw = capture::capture_window(xid);

            if (!raw)
                return false;

            GdkPixbuf *scaled = raw;

            if (size > 0)
            {
                scaled = get_gdk_pixbuf_scaled(
                    xid,
                    raw,
                    size,
                    size);

                g_object_unref(raw);

                if (!scaled)
                    return false;
            }

            image = Glib::wrap(scaled, true);

            return static_cast<bool>(image);
        }

        Glib::RefPtr<Gdk::Pixbuf> get_pixbuf_scaled(const Glib::RefPtr<Gdk::Pixbuf> &pixbuf,
                                                    const guint destWidth, const guint destHeight,
                                                    guint &scaledWidth, guint &scaledHeight)
        {
            if (!pixbuf)
            {
                return pixbuf;
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

            auto workarea = Position()->get_workarea();

            guint half_WindowWidth = workarea.get_width() / 3;
            guint half_WindowHeight = workarea.get_height() / 3;

            // ajust width size
            if (winWidth > half_WindowWidth)
            {
                scaledWidth = width - 6;
            }

            // ajust height size
            if (winHeight > half_WindowHeight)
            {
                scaledHeight = height - 4;
            }

            return pixbuf->scale_simple(scaledWidth, scaledHeight, Gdk::INTERP_BILINEAR);
        }

        // Additional method to center the image when it's smaller than the destination
        void centerImage(int &width, int &height, int maxWidth, int maxHeight)
        {
            if (width <= maxWidth && height <= maxHeight)
            {
                return; // No need to adjust
            }

            double scale = std::min(static_cast<double>(maxWidth) / width,
                                    static_cast<double>(maxHeight) / height);

            width = static_cast<int>(width * scale);
            height = static_cast<int>(height * scale);

            int x = (maxWidth - width) / 2;
            int y = (maxHeight - height) / 2;

            width += x;
            height += y;
        }

        //
        GdkPixbuf *get_gdk_pixbuf_scaled(gulong xid, const GdkPixbuf *pixbuf, const guint destWidth,
                                         const guint destHeight)
        {
            if (!pixbuf)
            {
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

            int scaledWidth = std::abs(winWidth * aspectRatio);
            int scaledHeight = std::abs(winHeight * aspectRatio);

            auto const workarea = Position()->get_workarea();

            guint half_WindowWidth = workarea.get_width() / 2;
            guint half_WindowHeight = workarea.get_height() / 2;

            const int ajust_value = 200;
            /*
            if (winWidth > half_WindowWidth + ajust_value)
            {
                scaledWidth = width;
            }

            if (winHeight > half_WindowHeight + ajust_value)
            {
                scaledHeight = height;
            }

            std::cout
                << "[Scale] source="
                << winWidth << "x" << winHeight
                << "  target="
                << scaledWidth << "x" << scaledHeight
                << '\n';
             */

            return gdk_pixbuf_scale_simple(pixbuf, scaledWidth, scaledHeight, GDK_INTERP_BILINEAR);
        }

        /**
         * Converts a GdkPixbuf to a Glib::RefPtr<Gdk::Pixbuf>.
         * DEPRECATED
         */
        const Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf *icon)
        {
            Glib::RefPtr<Gdk::Pixbuf> result;

            int width = gdk_pixbuf_get_width(icon);
            int height = gdk_pixbuf_get_height(icon);
            int stride = gdk_pixbuf_get_rowstride(icon);
            gboolean hasalpha = gdk_pixbuf_get_has_alpha(icon);
            int bitsaple = gdk_pixbuf_get_bits_per_sample(icon);
            guint8 *pdata;
            guchar *pixels = gdk_pixbuf_get_pixels(icon);

            pdata = static_cast<guint8 *>(pixels);

            result = Gdk::Pixbuf::create_from_data(pdata, Gdk::COLORSPACE_RGB, hasalpha, bitsaple,
                                                   width, height, stride);

            return result;
        }
    } // namespace pixbuf

} // namespace docklight
