#include "utils/pixbuf.h"
#include <glibmm/fileutils.h>
#include <gtkmm/iconinfo.h>
#include <gtkmm/icontheme.h>
DL_NS_BEGIN

namespace pixbuf_util
{
    // needs to keep the icon around.
    GdkPixbuf* referenced_icon = nullptr;

    const Glib::RefPtr<Gdk::Pixbuf> get_from_file(const std::string& filename,
                                                  int width, int height)
    {
        Glib::RefPtr<Gdk::Pixbuf> result;

        try {
            result =
                Gdk::Pixbuf::create_from_file(filename, width, height, true);
        } catch (const Glib::FileError& ex) {
            g_error("FileError: %s", ex.what().c_str());
        } catch (const Gdk::PixbufError& ex) {
            g_error("PixbufError: %s", ex.what().c_str());
        }

        return result;
    }

    const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(WnckWindow* window,
                                                    const string& icon_name,
                                                    int size)
    {
        if (icon_name.c_str() != nullptr) {
            GError* error = nullptr;
            GtkIconTheme* icon_theme = gtk_icon_theme_get_default();
            auto pixbuf =
                gtk_icon_theme_load_icon(icon_theme,
                                         icon_name.c_str(),  // icon name
                                         size,               // icon size
                                         GTK_ICON_LOOKUP_FORCE_SIZE, &error);
            if (!error) {
                return Glib::wrap(pixbuf, false);
            }

            g_error_free(error);
            error = nullptr;
        }

        auto icon = wnck_window_get_icon(window);
        return Glib::wrap(icon, false);

        /*auto const theme = Gtk::IconTheme::get_default();
        auto icon_info =
            theme->lookup_icon(icon_name, size,
        Gtk::ICON_LOOKUP_USE_BUILTIN);
        // try {
        if (icon_info) {
            if (icon_info.get_filename().c_str() == nullptr) {
                auto icon = icon_info.get_builtin_pixbuf();
                if (icon) {
                    return icon;
                }
            } else {
                return theme->load_icon(icon_name, 32, size,
                                        Gtk::ICON_LOOKUP_USE_BUILTIN);
            }
        }

        //  catch (...) {
        //       g_warning("Could not load icon name: %s\n",
        icon_name.c_str());
        //   }*/

        //        auto _referenced_icon = wnck_window_get_icon(window);
        // V      return Glib::wrap(_referenced_icon, false);
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

        result =
            Gdk::Pixbuf::create_from_data(pdata, Gdk::COLORSPACE_RGB, hasalpha,
                                          bitsaple, width, height, stride);

        return result;
    }
}  // namespace pixbuf_util

DL_NS_END

