#include "components/titlewindow.h"

#include "components/config.h"
#include "pango/pango-layout.h"
#include "utils/cairo.h"

namespace docklight
{

    TitleWindow::TitleWindow()
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
    }

    /**
     * Hide the window on destroy
     */
    TitleWindow::~TitleWindow()
    {
        hide();
        g_print("Free TitleWindow\n");
    }
    void TitleWindow::hide_now()
    {
        hide();
        m_visible = false;
    }

    void TitleWindow::show_at(int x, int y)
    {
        if (x && y) move(x, y);
        if (m_visible) return;

        show_all();
        m_visible = true;
    }
    void TitleWindow::set_text(const Glib::ustring text)
    {
        if (m_last_text != text) {
            m_last_text = text;
        } else {
            return;
        }

        m_Label.set_text(text);
        resize(2, this->get_height());
    }
    bool TitleWindow::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
    {
        auto m_theme = Config()->get_theme();

        Gdk::Rectangle rect = Gdk::Rectangle(0, 0, this->get_width(), this->get_height());
        cairo::fill(cr, m_theme.PanelTitle(), m_theme.PanelTitle(), rect);
        cr->fill();

        Glib::RefPtr<Pango::Layout> layout = m_Label.get_layout();
        pango_layout_set_alignment(layout->gobj(), PANGO_ALIGN_CENTER);

        //        / cairo::stroke(cr, m_theme.PanelTitleText(), rect);
        // cr->stroke();
        cr->set_source_rgba(m_theme.PanelTitleText().Stroke().Color::red,
                            m_theme.PanelTitleText().Stroke().Color::green,
                            m_theme.PanelTitleText().Stroke().Color::blue,
                            m_theme.PanelTitleText().Stroke().Color::alpha);

        cr->stroke();

        cr->move_to(6, 6);
        layout->show_in_cairo_context(cr);
        cr->reset_clip();

        return true;
    }
}  // namespace docklight

