//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 12:17 PM
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

#include "pango/pango-layout.h"

#include "TitleWindow.h"
#include "Configuration.h"
#include "Utilities.h"

/**
 * constructs a window POPUP contained a Label.
 */
TitleWindow::TitleWindow() :
Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
m_HBox(Gtk::ORIENTATION_HORIZONTAL, 5),
m_Label("", false)
{

    this->set_gravity(Gdk::Gravity::GRAVITY_STATIC);

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

    Gtk::Window::set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_TOOLTIP);
    m_HBox.set_margin_left(6);
    m_HBox.set_margin_right(6);
    m_HBox.set_margin_top(6);
    m_HBox.set_margin_bottom(6);

    //override_background_color(Gdk::RGBA("black"));
    //m_Label.override_color(Gdk::RGBA("white"), Gtk::STATE_FLAG_NORMAL);

    add(m_HBox);
    m_HBox.add(m_Label);


}

/**
 * Hide the window on destroy
 */
TitleWindow::~TitleWindow()
{
    hide();
}

/**
 * Set the tooltip-text to show
 * @param text the text to show
 */
void TitleWindow::setText(const Glib::ustring text)
{
    m_Label.set_text(text);
    resize( 2, get_height()); // Trick to auto resize the window
    show_all();
}

bool TitleWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{

    cr->set_source_rgba(
            m_Theme.PanelTitle().Fill().Color::red,
            m_Theme.PanelTitle().Fill().Color::green,
            m_Theme.PanelTitle().Fill().Color::blue,
            m_Theme.PanelTitle().Fill().Color::alpha);
    Utilities::RoundedRectangle(cr, 0, 0, this->get_width(), this->get_height(),m_Theme.PanelTitle().Ratio());
    cr->fill();


    cr->set_source_rgba(
            m_Theme.PanelTitle().Stroke().Color::red,
            m_Theme.PanelTitle().Stroke().Color::green,
            m_Theme.PanelTitle().Stroke().Color::blue,
            m_Theme.PanelTitle().Stroke().Color::alpha);

    cr->set_line_width(m_Theme.PanelTitle().LineWidth());
    Utilities::RoundedRectangle(cr, 0, 0, this->get_width(), this->get_height(),m_Theme.PanelTitle().Ratio());
    cr->stroke();

    Glib::RefPtr<Pango::Layout> layout = create_pango_layout(m_Label.get_text());
    pango_layout_set_alignment(layout->gobj(),PANGO_ALIGN_CENTER );

    cr->set_source_rgba(
            m_Theme.PanelTitleText().Stroke().Color::red,
            m_Theme.PanelTitleText().Stroke().Color::green,
            m_Theme.PanelTitleText().Stroke().Color::blue,
            m_Theme.PanelTitleText().Stroke().Color::alpha)
            ;
    cr->move_to(6, 6);
    layout->show_in_cairo_context(cr);
    cr->reset_clip(); // Reset the clipping

}
