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


#include "IconLoader.h"
#include "Configuration.h"
#include "Utilities.h"

#include "Launcher.h"
#include <gtkmm/icontheme.h>

namespace IconLoader
{
    // https://developer.gnome.org/gtk3/stable/GtkIconTheme.html#GtkIconLookupFlags
    // enum GtkIconLookupFlags
    // Used to specify options for gtk_icon_theme_lookup_icon()
    GtkIconLookupFlags flags = GTK_ICON_LOOKUP_GENERIC_FALLBACK;

    // needs to keep the icon around.
    GdkPixbuf *referenceIcon = nullptr;

    // the desired icon size.
    // The resulting icon may not be exactly this size.
    gint size = DEF_ICONMAXSIZE;


    Glib::RefPtr<Gdk::Pixbuf> GetIconByAppName(const char* appname,std::string &theme_iconname)
    {
        Glib::RefPtr<Gdk::Pixbuf> m_image;
        GError *error = NULL;
        GKeyFile *key_file = g_key_file_new();
        if (!Launcher::getDesktopFile(key_file, appname)) {

            g_key_file_free(key_file);
            return NULLPB;
        }

         // check if the Icon Desktop Entry name exists
        gchar* iconname = g_key_file_get_string(key_file,
                "Desktop Entry", "Icon", &error);

        if (iconname == NULL) {
            if (error) {
                g_warning("Icon Desktop Entry not found: %s %s", iconname, error->message);
                g_error_free(error);
                error = NULL;
            }

            g_key_file_free(key_file);
            return NULLPB;
        }

        g_key_file_free(key_file);


         GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
         auto pixbuf = gtk_icon_theme_load_icon (icon_theme,
                                   iconname, // icon name
                                   48, // icon size
                                   GTK_ICON_LOOKUP_FORCE_SIZE,  // flags // https://developer.gnome.org/gtk3/stable/GtkIconTheme.html#GtkIconLookupFlags
                                   &error);
        if(error){
            g_error_free(error);
            error = NULL;
           return NULLPB;
        }

        theme_iconname = iconname;
        m_image = IconLoader::PixbufConvert(pixbuf);

        return m_image;

    }

    /**
     * Get the window icons as Gdk::Pixbuf.
     * Icons and themes are looked for in a set of directories.
     * By default, apps should look in $HOME/.icons (for backwards compatibility),
     * in $XDG_DATA_DIRS/icons and in /usr/share/pixmaps,/usr/share/icons (in that order).
     * https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html

     * @param window
     * @return Glib::RefPtr<Gdk::Pixbuf>
     */
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window)
    {
        std::string theme_iconname;
        return GetWindowIcon(window, referenceIcon,theme_iconname);

    }
    Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window,std::string &theme_iconname)
    {
        return GetWindowIcon(window, referenceIcon,theme_iconname);
    }

    /**
     * Get the window icons as Gdk::Pixbuf.
     * Icons and themes are looked for in a set of directories.
     * By default, apps should look in $HOME/.icons (for backwards compatibility),
     * in $XDG_DATA_DIRS/icons and in /usr/share/pixmaps,/usr/share/icons (in that order).
     * https://specifications.freedesktop.org/icon-theme-spec/icon-theme-spec-latest.html

     * @param window
     * @param icon The caller should reference the returned GdkPixbuf if it needs to keep the icon around.
     * @return Glib::RefPtr<Gdk::Pixbuf>
     */
  Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window, GdkPixbuf *icon,std::string &theme_iconname)
    {
        Glib::RefPtr<Gdk::Pixbuf> result = NULLPB;


        std::string the_appname;
        std::string the_instancename;
        std::string the_groupname;
        std::string the_titlename;

        if (Launcher::getAppNameByWindow(window,
                the_appname,
                the_instancename,
                the_groupname,
                the_titlename) == FALSE) {

            return PixbufConvert(wnck_window_get_icon(window));
        }

        GError *error = NULL;
        GKeyFile *key_file = g_key_file_new();
        if (!Launcher::getDesktopFile(key_file, the_groupname.c_str())) {

            g_key_file_free(key_file);
            return PixbufConvert(wnck_window_get_icon(window));
        }

        // check if the Icon Desktop Entry name exists
        gchar* iconname = g_key_file_get_string(key_file,
                "Desktop Entry", "Icon", &error);

        if (iconname == NULL) {
            if (error) {
                g_warning("Icon Desktop Entry not found: %s %s", iconname, error->message);
                g_error_free(error);
                error = NULL;
            }

            g_key_file_free(key_file);
            return PixbufConvert(wnck_window_get_icon(window));
        }

        g_key_file_free(key_file);
        theme_iconname = iconname;

        if (strstr(iconname, "/") != NULL) {

            if (g_file_test(iconname, GFileTest::G_FILE_TEST_EXISTS)) {
                try {
                    Glib::RefPtr<Gdk::Pixbuf> fromfile;
                    return fromfile->create_from_file(iconname);

                } catch (Glib::FileError fex) {
                    g_warning("Icon file: %s from desktop: %s could not be found.",
                            iconname, the_groupname.c_str());

                } catch (Gdk::PixbufError bex) {
                    g_warning("Icon file: %s from desktop: %s PixbufError.",
                            iconname, the_groupname.c_str());
                }
            }

        }


        GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
        if (icon_theme != NULL && gtk_icon_theme_has_icon(icon_theme, iconname)) {

            icon = gtk_icon_theme_load_icon(icon_theme, iconname,
                    size, flags, &error);


            g_print("Load Icon from Theme %s\n", iconname);
            if (icon == NULL) {
                icon = wnck_window_get_icon(window);
                g_print("Load Icon from window %s\n", iconname);
            }
        }

        if( icon == NULL ) {
            icon = icon = wnck_window_get_icon(window);
            g_print("Load Icon from window %s\n", iconname);
        }

        result = PixbufConvert(icon);
        if( G_IS_OBJECT (icon))
            g_object_unref(icon);
        return result;
    }


    /**
     * Converts a GdkPixbuf to a Glib::RefPtr<Gdk::Pixbuf>.
     * @param icon
     * @return
     */
    Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon)
    {
        Glib::RefPtr<Gdk::Pixbuf> result = NULLPB;

        int width = gdk_pixbuf_get_width(icon);
        int height = gdk_pixbuf_get_height(icon);
        int stride = gdk_pixbuf_get_rowstride(icon);
        gboolean hasalpha = gdk_pixbuf_get_has_alpha(icon);
        int bitsaple = gdk_pixbuf_get_bits_per_sample(icon);
        guint8 *pdata;
        guchar *pixels = gdk_pixbuf_get_pixels(icon);

        pdata = static_cast<guint8*> (pixels);

        result = Gdk::Pixbuf::create_from_data(pdata,
                Gdk::COLORSPACE_RGB, hasalpha, bitsaple,
                width, height, stride);

        return result;
    }
}


