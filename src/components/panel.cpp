#include "components/panel.h"
#include <gdkmm/general.h>  // set_source_pixbuf()
#include <glibmm/main.h>
#include <glibmm/timer.h>
#include "appwindow.h"
#include "components/config.h"
#include "utils/cairo.h"

DL_NS_BEGIN

Panel::Panel()
{
    m_app_updater.signal_update().connect(
        sigc::mem_fun(this, &Panel::on_appupdater_update));

    // https://developer.gnome.org/gtkmm-tutorial/stable/sec-timeouts.html.en
    m_sigc_connection = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &Panel::on_timeout_draw), 1000 / 8);

    m_sigc_connection.disconnect();
}

Panel::~Panel() {}

void Panel::on_appupdater_update(bool a, int b)
{
    g_print("Update signal\n");
    AppWindow::update();

    m_draw_required = true;
    on_timeout_draw();
}

int Panel::get_required_size()
{
    int separators = config::get_separator_margin() *
                     (m_app_updater.m_dock_items.size() - 1);
    //        config::get_separator_margin();
    //
    g_print("Size %d\n", (int)m_app_updater.m_dock_items.size());
    return (m_app_updater.m_dock_items.size() * 48) +
           config::get_window_start_end_margin() + separators;
}

bool Panel::on_timeout_draw()
{
    if (m_draw_required) {
        Gtk::Widget::queue_draw();
        m_draw_required = false;
    }
    return true;
}

inline void Panel::get_item_position(const dock_item_type_t item_typ, int& x,
                                     int& y, int& width, int& height)
{
    static int nextsize = 0;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        if (x == 0) {
            y = 4;
            x = config::get_window_start_end_margin() / 2;
            nextsize = width;
            return;
        }

        x += nextsize + config::get_separator_margin();
        nextsize = width;
    }
    return;

    /*if (DockWindow::is_Horizontal()) {
        if (x == 0) {
            y = DockWindow::get_DockWindowHeight() / 2 - height / 2;
            x = DockWindow::get_dockWindowStartEndMargin() / 2;
            nextsize = width;
            return;
        }

        // if the item is a separator the width is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        if (dockType == DockItemType::Separator) {
            x += nextsize + Configuration::get_separatorMargin();
            nextsize = width;
            return;
        }

        x += nextsize + Configuration::get_separatorMargin();
        nextsize = width;
    } else {
        if (y == 0) {
            x = Configuration::get_dockWindowSize() / 2 - width / 2;
            y = DockWindow::get_dockWindowStartEndMargin() / 2;
            nextsize = height;
            return;
        }

        // if the item is a separator the wdth is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        if (dockType == DockItemType::Separator) {
            y += nextsize + Configuration::get_separatorMargin();
            height = nextsize = width;
            width = Configuration::get_CellWidth();
            return;
        }

        y += nextsize + Configuration::get_separatorMargin();
        nextsize = height;
    }*/
}
bool Panel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    this->draw_panel(cr);
    this->draw_items(cr);
    return true;
}

void Panel::draw_panel(const Cairo::RefPtr<Cairo::Context>& cr)
{
    cr->set_source_rgba(1, 0, 0, 0);
    cr->paint();
}

void Panel::draw_items(const Cairo::RefPtr<Cairo::Context>& cr)
{
    int y = 0;
    int x = 0;
    int width = 0;
    int height = 0;

    size_t items_count = m_app_updater.m_dock_items.size();

    // Draw all items with cairo
    for (size_t idx = 0; idx < items_count; idx++) {
        auto const item = m_app_updater.m_dock_items[idx];

        width = item->get_width();
        height = item->get_height();

        this->get_item_position(dock_item_type_t::single, x, y, width, height);

        //     g_print("Draw %d\n", x);
        item->set_index(idx);
        item->set_x(x);
        item->set_y(y);

        // draw cell
        cr->set_source_rgba(0, 0, 1, 0);
        cairo_util::rounded_rectangle(cr, x, y, width, height, 3);
        cr->stroke();

        // draw icon
        auto const image =
            item->get_image()->scale_simple(48, 48, Gdk::INTERP_BILINEAR);
        Gdk::Cairo::set_source_pixbuf(cr, image, x, y);
        cr->paint();
    }
}

DL_NS_END
