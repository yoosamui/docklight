#include "utils/pixbuf.h"

namespace docklight
{
    namespace pixbuf
    {
        // TODO get size array from theme to verify size.
        const Glib::RefPtr<Gdk::Pixbuf> convert_window_icon(GdkPixbuf* icon, guint size)
        {
            Glib::RefPtr<Gdk::Pixbuf> pixbuf;

            if (icon) {
                pixbuf = Glib::wrap(icon, true)->scale_simple(size, size, Gdk::INTERP_BILINEAR);
                g_object_unref(icon);
            }

            return pixbuf;
        }

        // TODO get size array from them to verify size.
        const Glib::RefPtr<Gdk::Pixbuf> convert_gioicon(const Glib::RefPtr<Gio::Icon>& icon,
                                                        guint size)
        {
            Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

            Gtk::IconInfo iconInfo = theme->lookup_icon(icon, size, Gtk::ICON_LOOKUP_USE_BUILTIN);

            Glib::RefPtr<Gdk::Pixbuf> pixbuf = iconInfo.load_icon();
            return pixbuf;
        }
    }  // namespace pixbuf
}  // namespace docklight
