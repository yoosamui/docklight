/* 
 * File:   IconWindow.cpp
 * Author: yoo
 * 
 * Created on March 23, 2019, 1:27 PM
 */

#include "IconWindow.h"

IconWindow::IconWindow(){ }

IconWindow::~IconWindow(){ }

void IconWindow::setImage(const Glib::RefPtr<Gdk::Pixbuf> image)
{
    this->m_image = image;
}

bool IconWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (m_image == NULLPB) {
        return true;
    }
    Gdk::Cairo::set_source_pixbuf(cr, this->m_image, 0, 0);
    cr->paint();

    return true;
}