#include "components/dock_item.h"
#include <cairomm/context.h>
#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/main.h>

DL_NS_BEGIN

dock_item::dock_item()
{
    //    this->add(m_event_box);  //.add(*this);
}

void dock_item::set_image(const Glib::RefPtr<Gdk::Pixbuf>& image)
{
    if (!image) {
        return;
    }

    g_print("Image set w................\n");
    this->set(image);
}

/*bool dock_item::on_timeout()
{
    // force our program to redraw the entire clock.
    // auto win = get_window();
    // if (win) {
    // g_print("OnTimeout ....:.\n");
    // Gdk::Rectangle r(0, 0, get_allocation().get_width(),
    // get_allocation().get_height());
    // win->invalidate_rect(r, false);
    //}

    Gtk::Widget::queue_draw();
    return true;
}

bool dock_item::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    g_print("On Draw....:.\n");
    if (!m_image) {
        g_print("Image s NULL.....\n");
        return true;
    }

    Gdk::Cairo::set_source_pixbuf(cr, m_image, 0, 0);
    cr->paint();
    return true;
}*/

DL_NS_END
