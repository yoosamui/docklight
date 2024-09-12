#include "utils/pixbuf.h"

namespace docklight
{
    namespace pixbuf
    {
        const Glib::RefPtr<Gdk::Pixbuf> get_icon(const gchar* icon_name, GdkPixbuf* window_icon,
                                                 guint size)
        {
            Glib::RefPtr<Gdk::Pixbuf> pixbuf;
            GtkIconTheme* icon_theme = gtk_icon_theme_get_default();

            GError* error = nullptr;
            GdkPixbuf* icon = gtk_icon_theme_load_icon(icon_theme,
                                                       icon_name,                    // icon name
                                                       size,                         // icon size
                                                       GTK_ICON_LOOKUP_USE_BUILTIN,  // flags //
                                                       &error);

            if (error) {
                g_error_free(error);
                error = nullptr;

                // try to load from window;
                if (window_icon) {
                    pixbuf = get_window_icon(window_icon, size);
                    // pixbuf->save("/home/yoo/TEMP/JUAN", "png");
                    // return pixbuf;
                }

                if (!pixbuf) {
                    g_warning("(pixbuf::get_icon) icon '%s' cannot be found!.\n", icon_name);
                }

                // if (pixbuf) {
                // g_print("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAi\n");
                // pixbuf->save("/home/yoo/TEMP/JUAN", "png");
                //} else {
                //}

            } else {
                if (icon) {
                    pixbuf = Glib::wrap(icon);
                }
            }

            //  if (G_IS_OBJECT(icon)) g_object_unref(icon);
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
