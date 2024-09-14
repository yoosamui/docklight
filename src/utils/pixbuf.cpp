#include "utils/pixbuf.h"

namespace docklight
{
    namespace pixbuf
    {

        const Glib::RefPtr<Gdk::Pixbuf> get_theme_icon(const gchar* icon_name,
                                                       GdkPixbuf* window_icon, guint size)
        {
            Glib::RefPtr<Gdk::Pixbuf> pixbuf;

            // const Glib::RefPtr<Gdk::Screen> screen =
            // Gdk::Display::get_default()->get_default_screen();

            //        const Glib::RefPtr<Gdk::Display> display = Gdk::Display::get_default();
            // Glib::RefPtr<Gdk::IconTheme> theme = Gtk::IconTheme::get_for_display(&display);
            // A unique Gtk::IconTheme associated with the given display. This icon theme is
            // associated with the display and can be used as long as the display is open. Do not
            // ref or unref it.
            //  auto theme = Gtk::IconTheme::get_for_display(&display);

            ///    Glib::RefPtr<IconTheme> theme = get_for_screen(&screen);
            // Glib::RefPtr<Gdk::IconTheme> theme = Gdk::IconTheme::get_for_screen(screen);
            // auto theme = Gdk::IconTheme::get_for_screen(screen);
            //            auto theme = screen->get_for_screen(screen);
            //.auto icons = Gtk::IconTheme::get_icon_names();
            ///////////////////////
            /**
             * This icon theme is associated with
             * the screen and can be used as long as the screen
             * is open. Do not ref or unref it.
             */
            Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();
            std::vector<int> icon_sizes = theme->get_icon_sizes("icon_name_coms_here");
            /**
             *
             * @return A list holding the names of all the
             * icons in the theme.
             */

            std::vector<Glib::ustring> icon_names = theme->list_icons();

            // Glib::RefPtr<Gdk::Pixbuf> icon =
            //     load_icon("const Glib::ustring& icon_name", "int size");

            //////////////////////////

            //  int scale}; //, IconLookupFlags flags =  (IconLookupFlags)0) const;
            //                const Lists the names of icons in the current icon theme.
            //  const Glib::RefPtr<Gdk::Screen> screen =
            //  Gdk::Display::get_default()->get_default_screen();

            // Gets the icon theme object associated with display.

            return pixbuf;
        }
        /*
         * The caller must always check the return pointer for null.
         */
        const Glib::RefPtr<Gdk::Pixbuf> get_theme_iconX(const gchar* icon_name,
                                                        GdkPixbuf* window_icon, guint size)
        {
            Glib::RefPtr<Gdk::Pixbuf> pixbuf;
            GtkIconTheme* icon_theme = gtk_icon_theme_get_default();

            // We check if  the icon_name pointer is null,
            // means that no desktop file exists, and therefore no theme.
            // We trying to load the window icon;
            /*if (icon_name == nullptr && window_icon) {
                pixbuf = get_window_icon(window_icon, size);
                if (!pixbuf) {
                    g_critical("(pixbuf::get_icon)  cannot be found! NO_DESKTOP_FILE.\n");
                }

                return pixbuf;
            }*/
            // g_print("------%s\n", icon_name);
            // size = 64;
            //  We lookup for the current theme.
            //  At this point we have a valid desktop file.
            GError* error = nullptr;

            GdkPixbuf* icon;
            icon = gtk_icon_theme_load_icon(icon_theme,
                                            icon_name,                    // icon name
                                            size,                         // icon size
                                            GTK_ICON_LOOKUP_USE_BUILTIN,  // flags //
                                            &error);

            if (error) {
                g_error_free(error);
                error = nullptr;

                if (window_icon) {
                    pixbuf = get_window_icon(window_icon, size);
                    if (!pixbuf) {
                        g_critical("(pixbuf::get_icon)  cannot be found! NO_DESKTOP_FILE.\n");
                    }

                    return pixbuf;
                }

                if (!pixbuf) {
                    g_warning("(pixbuf::get_icon) icon '%s' cannot be found!.\n", icon_name);
                }
            }

            if (icon) {
                pixbuf = Glib::wrap(icon);
                //    if (G_IS_OBJECT(icon)) g_object_unref(icon);
            }

            return pixbuf;
        }

        // TODO get size array from theme to verify size.
        const Glib::RefPtr<Gdk::Pixbuf> get_window_icon(GdkPixbuf* icon, guint size)
        {
            Glib::RefPtr<Gdk::Pixbuf> pixbuf;

            if (icon) {
                pixbuf = Glib::wrap(icon)->scale_simple(size, size, Gdk::INTERP_BILINEAR);
            }

            return pixbuf;
        }

        // TODO get size array from them to verify size.
        // const Glib::RefPtr<Gdk::Pixbuf> convert_gioicon(const Glib::RefPtr<Gio::Icon>& icon,
        // guint size)
        //{
        // Glib::RefPtr<Gdk::Pixbuf> pixbuf;
        // Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

        // if (theme) {
        // Gtk::IconInfo iconInfo =
        // theme->lookup_icon(icon, size, Gtk::ICON_LOOKUP_USE_BUILTIN);
        // if (iconInfo) {
        ////-TODO CRASH HERE WHEN THE THEN ICON THEME CHANGES
        // pixbuf = iconInfo.load_icon();
        //}
        //}
        // return pixbuf;
        //}
    }  // namespace pixbuf
}  // namespace docklight
