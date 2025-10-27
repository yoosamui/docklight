// g++ program.cpp -o transparent `pkg-config --cflags --libs gtkmm-3.0` -lX11 -lXext

// clang-format off

#include <gtkmm.h>
#include <gdk/gdkx.h>       // For GDK_WINDOW_XDISPLAY, GDK_WINDOW_XID

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>
#include <iostream>
// clang-format on

class TransparentWindow : public Gtk::Window
{
  public:
    TransparentWindow();

  protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    bool on_key_press_event(GdkEventKey* event) override;
    bool on_motion_notify_event(GdkEventMotion* event) override;
    bool on_button_press_event(GdkEventButton* event) override;

  private:
    Gtk::Label label;
    bool click_through_enabled = true;  // small rectangle is click-through

    void apply_clickable_region();
    void toggle_click_through()
    {
        click_through_enabled = !click_through_enabled;
        apply_clickable_region();
    }
};

TransparentWindow::TransparentWindow()
{
    set_title("Transparent Click-Through Window");
    set_default_size(400, 300);
    set_app_paintable(true);
    set_decorated(false);
    set_keep_above(true);
    set_position(Gtk::WIN_POS_CENTER);

    // Transparent RGBA visual
    auto screen = Gdk::Screen::get_default();
    if (screen) {
        auto visual = screen->get_rgba_visual();
        if (visual) gtk_widget_set_visual(GTK_WIDGET(gobj()), visual->gobj());
    }

    // Label setup
    label.set_text("I AM HERE");
    label.override_color(Gdk::RGBA("white"));
    label.override_font(Pango::FontDescription("Sans Bold 28"));
    label.set_halign(Gtk::ALIGN_CENTER);
    label.set_valign(Gtk::ALIGN_CENTER);
    add(label);

    show_all_children();

    // Apply initial click-through after window mapped
    signal_map_event().connect([this](GdkEventAny*) {
        apply_clickable_region();
        return false;
    });

    // Enable pointer motion and button press events
    add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK);
    signal_motion_notify_event().connect(
        sigc::mem_fun(*this, &TransparentWindow::on_motion_notify_event));
    signal_button_press_event().connect(
        sigc::mem_fun(*this, &TransparentWindow::on_button_press_event));
}

bool TransparentWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    // Fully transparent background
    cr->set_source_rgba(0, 0, 0, 0.5);
    cr->set_operator(Cairo::OPERATOR_SOURCE);
    cr->paint();
    return Gtk::Window::on_draw(cr);
}

bool TransparentWindow::on_key_press_event(GdkEventKey* event)
{
    if (!event) return false;

    switch (event->keyval) {
        case GDK_KEY_Escape:
            hide();
            return true;

        case GDK_KEY_space:
            toggle_click_through();
            g_print("Click-through rectangle %s\n", click_through_enabled ? "ON" : "OFF");
            return true;
    }

    return Gtk::Window::on_key_press_event(event);
}

// Mouse motion handler: only report outside click-through rectangle
bool TransparentWindow::on_motion_notify_event(GdkEventMotion* event)
{
    /*if (!event) return false;

    if (!(click_through_enabled && event->x >= 0 && event->x < 10 && event->y >= 0 &&
          event->y < 10)) {
        g_print("Mouse moved: x=%f, y=%f\n", event->x, event->y);
    }*/

    return false;  // allow default processing
}

// Mouse left-click handler: only report outside click-through rectangle
bool TransparentWindow::on_button_press_event(GdkEventButton* event)
{
    if (!event) return false;

    if (event->button == 1) {  // left click
        // if (!(click_through_enabled && event->x >= 0 && event->x < 100 && event->y >= 0 &&
        // event->y < 100)) {
        // g_print("Left click: x=%f, y=%f\n", event->x, event->y);
        //}
        g_print("Left click: x=%f, y=%f\n", event->x, event->y);
    }

    return false;  // allow default processing
}

// Apply input region: small rectangle click-through, rest receives input
void TransparentWindow::apply_clickable_region()
{
#ifdef GDK_WINDOWING_X11
    auto gdk_window = get_window();
    if (!gdk_window) return;

    Display* dpy = GDK_WINDOW_XDISPLAY(gdk_window->gobj());
    ::Window xid = GDK_WINDOW_XID(gdk_window->gobj());

    int w = get_width();
    int h = get_height();

    // Full window receives input
    Region region = XCreateRegion();
    XRectangle full{0, 0, static_cast<unsigned short>(w), static_cast<unsigned short>(h)};
    XUnionRectWithRegion(&full, region, region);

    if (click_through_enabled) {
        // Subtract small rectangle (10x10 top-left) → click-through
        XRectangle hole{0, 0, static_cast<unsigned short>(w), 100};
        Region hole_region = XCreateRegion();
        XUnionRectWithRegion(&hole, hole_region, hole_region);
        XSubtractRegion(region, hole_region, region);
        XDestroyRegion(hole_region);
    } else {
        // Full window receives input → region covers whole window
        XRectangle full{0, 0, static_cast<unsigned short>(w), static_cast<unsigned short>(h)};
        XUnionRectWithRegion(&full, region, region);
    }

    // Apply region as input mask
    XShapeCombineRegion(dpy, xid, ShapeInput, 0, 0, region, ShapeSet);
    XDestroyRegion(region);
    XFlush(dpy);
#endif
}

int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create(argc, argv, "org.example.transparent");
    TransparentWindow win;
    return app->run(win);
}

