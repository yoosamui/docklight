#include "titlewindow.h"
#include "components/config.h"
#include "pango/pango-layout.h"
#include "utils/cairo.h"

DL_NS_BEGIN

title_window::title_window()
    : Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
      m_HBox(Gtk::ORIENTATION_HORIZONTAL, 5),
      m_Label("", false)
{
    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

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
title_window::~title_window()
{
    hide();
    g_print("Free title_window\n");
}
void title_window::set_text(const Glib::ustring text)
{
    m_Label.set_text(text);
    resize(2, this->get_height());
    show_all();
}
bool title_window::on_draw(const Cairo::RefPtr<Cairo::Context> &cr)
{
    // cr->set_source_rgba(m_Theme.PanelTitle().Fill().Color::red,
    // m_Theme.PanelTitle().Fill().Color::green,
    // m_Theme.PanelTitle().Fill().Color::blue,
    // m_Theme.PanelTitle().Fill().Color::alpha);
    cairo_util::rounded_rectangle(cr, 0, 0, this->get_width(),
                                  this->get_height(),
                                  3.0 /*  m_Theme.PanelTitle().Ratio()*/);
    cr->fill();

    // cr->set_source_rgba(m_Theme.PanelTitle().Stroke().Color::red,
    // m_Theme.PanelTitle().Stroke().Color::green,
    // m_Theme.PanelTitle().Stroke().Color::blue,
    // m_Theme.PanelTitle().Stroke().Color::alpha);

    cr->set_line_width(1.5 /*m_Theme.PanelTitle().LineWidth()*/);
    cairo_util::rounded_rectangle(cr, 0, 0, this->get_width(),
                                  this->get_height(),
                                  3.0 /* m_Theme.PanelTitle().Ratio()*/);
    cr->stroke();

    Glib::RefPtr<Pango::Layout> layout =
        m_Label.get_layout();  // pcreate_pango_layout(m_text);
    pango_layout_set_alignment(layout->gobj(), PANGO_ALIGN_CENTER);

    // cr->set_source_rgba(m_Theme.PanelTitleText().Stroke().Color::red,
    // m_Theme.PanelTitleText().Stroke().Color::green,
    // m_Theme.PanelTitleText().Stroke().Color::blue,
    // m_Theme.PanelTitleText().Stroke().Color::alpha);
    cr->set_source_rgba(1, 1, 1, 1);
    cr->move_to(6, 6);
    layout->show_in_cairo_context(cr);
    cr->reset_clip();  // Reset the clipping

    return true;
}

DL_NS_END

