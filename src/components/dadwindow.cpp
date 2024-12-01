#include "components/dadwindow.h"

#include "components/config.h"
#include "pango/pango-layout.h"
#include "utils/cairo.h"

namespace docklight
{

    DADWindow::DADWindow(Glib::RefPtr<Gdk::Pixbuf> icon)
        : Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
          m_HBox(Gtk::ORIENTATION_HORIZONTAL, 5),
          m_Label("", false)
    {
        //    m_theme = config::get_theme();

        GdkScreen *screen;
        GdkVisual *visual;

        gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
        screen = gdk_screen_get_default();
        visual = gdk_screen_get_rgba_visual(screen);

        if (visual != NULL && gdk_screen_is_composited(screen)) {
            gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
        }

        set_resizable(true);
        set_skip_taskbar_hint(true);
        set_skip_pager_hint(true);
        set_keep_above(true);
        set_keep_above(true);

        font.set_family("System");
        font.set_size(8 * PANGO_SCALE);
        font.set_weight(Pango::WEIGHT_NORMAL);

        // Gtk::Window::set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_TOOLTIP);
        m_HBox.set_margin_left(6);
        m_HBox.set_margin_right(6);
        m_HBox.set_margin_top(6);
        m_HBox.set_margin_bottom(6);

        add(m_HBox);
        m_HBox.add(m_Label);

        m_icon = icon;

        m_size = Config()->get_icon_size();
        resize(m_size, m_size);
    }

    /**
     * Hide the window on destroy
     */
    DADWindow::~DADWindow()
    {
        hide();
        //  m_visible = false;
        g_print("Free DADWindow\n");
    }

    void DADWindow::close_now()
    {
        close();
        m_visible = false;
    }

    void DADWindow::move_at(int x, int y)
    {
        x -= m_size / 2;
        y -= m_size / 2;

        move(x, y);
        m_x = x;
        m_y = y;
    }

    void DADWindow::show_at(int dockitem_index)
    {
        int xx, yy;

        auto area = Config()->get_dock_area();
        if (Config()->get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            xx = Position()->get_x() + dockitem_index * area;
            yy = Position()->get_y();

        } else {
            yy = Position()->get_y() + dockitem_index * area;
            xx = Position()->get_x();
        }

        int margin = Config()->get_dock_area_margin() / 2;

        int x = xx + margin;
        int y = yy + margin;

        move(x, y);

        show_all();
        m_visible = true;
    }

    bool DADWindow::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
    {
        cr->set_source_rgba(1, 1, 1, 0);
        Gdk::Cairo::set_source_pixbuf(cr, m_icon, 0, 0);
        cr->paint();

        return true;
    }
}  // namespace docklight

