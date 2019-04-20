//*****************************************************************
//
//  Copyright (C) 2019 Juan R. Gonzalez
//  j-gonzalez@email.de
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

#include "Configuration.h"
#include "Utilities.h"
#include "DragAndDropWindow.h"
#include "DockWindow.h"
#include <gtkmm.h>

DragAndDropWindow::DragAndDropWindow():Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
{
    set_size_request(50, 50);
    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &DragAndDropWindow::on_timeoutDraw), 1000/60);
}

void DragAndDropWindow::Show(const Glib::RefPtr<Gdk::Pixbuf>& icon, DockItem* item, Gdk::Point mousePoint)
{
    this->m_item = item;
    guint width = item->get_Width() - CELL_MARGIN;
    guint height = item->get_Height() - CELL_MARGIN;

    this->m_icon = icon->scale_simple(width, height, Gdk::INTERP_BILINEAR);

    this->m_mousePoint.set_x(mousePoint.get_x());
    this->m_mousePoint.set_y(mousePoint.get_y());

    this->resize(width, height);

    if (Move(m_mousePoint.get_x(),  m_mousePoint.get_y())) {
        Gtk::Widget::queue_draw();
        this->show_now();
    }
}

bool DragAndDropWindow::Move(int x, int y)
{
    int mouseX;
    int mouseY;
    bool found = Utilities::getMousePosition(mouseX, mouseY);

    if (found){
        this->move(mouseX - x + m_item->m_posX + CELL_MARGIN / 2 ,  mouseY - y  +  m_item->m_posY + 1);
    }

    return found;
}

bool DragAndDropWindow::on_timeoutDraw()
{
    Move(m_mousePoint.get_x(), m_mousePoint.get_y());
    return true;
}

bool DragAndDropWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (this->m_icon != NULLPB){
        Gdk::Cairo::set_source_pixbuf(cr, this->m_icon, 0, 0);
        cr->paint();
    }
    return true;
}




