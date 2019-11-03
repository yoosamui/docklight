#include "panelpreview.h"

DL_NS_BEGIN

Panel_preview::Panel_preview() : Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
{
    // Set up the top-level window.
    set_title("PanelPreview");
    set_decorated(false);
    set_app_paintable(true);

    // clang-format off
    // Set masks for mouse events
    add_events(
            Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
            Gdk::ENTER_NOTIFY_MASK |
            Gdk::LEAVE_NOTIFY_MASK |
            Gdk::POINTER_MOTION_MASK);

    // clang-format on

    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    // m_font.set_family("System");
    // m_font.set_size(8 * PANGO_SCALE);
    // m_font.set_weight(Pango::WEIGHT_NORMAL);
}

Panel_preview::~Panel_preview() {}

DL_NS_END

