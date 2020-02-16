#include "panelrender.h"
#include <gdkmm/general.h>  // set_source_pixbuf()
#include "utils/cairo.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/system.h"

DL_NS_BEGIN

#define DEF_ICONNAME "data/images/docklight.home.ico"
panel_static_members_t Panel_render::m_stm;

Panel_render::Panel_render() {}

void Panel_render::init()
{
    m_theme = config::get_theme();
}

void Panel_render::draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    this->draw_panel(cr);
    this->draw_items(cr);
    this->draw_title();
}

void Panel_render::load_home_icon(int icon_size)
{
    string filename(system_util::get_current_path(DEF_ICONNAME));
    auto const home_pixbuf = pixbuf_util::get_from_file(filename, icon_size, icon_size);

    auto const home_item = AppUpdater::m_dockitems[0];
    home_item->set_image(home_pixbuf);
}
// https://www.cairographics.org/manual/cairo-cairo-pattern-t.html#cairo-pattern-create-linear
void Panel_render::draw_panel(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Gdk::Rectangle rect = position_util::get_appwindow_geometry();
    rect.set_x(m_offset_x);
    rect.set_y(m_offset_y);

    cairo_util::fill(cr, m_theme.Panel(), m_theme.PanelGradient(), rect);
    //    cr->paint();
    cairo_util::stroke(cr, m_theme.Panel(), rect);
}

void Panel_render::draw_items(const Cairo::RefPtr<Cairo::Context>& cr)
{
    int y = 0;
    int x = 0;
    int width = 0;
    int height = 0;
    int initial_icon_size = config::get_icon_size();
    int icon_size = initial_icon_size;
    Gtk::Orientation orientation = config::get_dock_orientation();
    int area = position_util::get_area();
    size_t items_count = m_appupdater.m_dockitems.size();

    // Draw all items with cairo
    for (size_t idx = 0; idx < items_count; idx++) {
        auto const item = m_appupdater.m_dockitems[idx];

        width = item->get_width();
        height = item->get_height();

        if (width < 1 || height < 1) {
            continue;
        }

        this->get_item_position(dock_item_type_t::launcher, x, y, width, height);

        item->set_x(x);
        item->set_y(y);

        if (orientation == Gtk::ORIENTATION_HORIZONTAL) {
            y = m_offset_y;
        } else {
            x = m_offset_x;
        }

        // separator
        this->draw_separator(cr, item, x, y, width, height, orientation);

        // cell
        this->draw_cell(cr, item, x, y, width, height, area, orientation);

        // drag indicator
        this->draw_drag_indicator(cr, item, x, y, idx, width, height, orientation);

        // draw active window selector
        this->draw_active_window_indicator(cr, x, y, idx, width, height);

        // icon + selector
        auto const icon = item->get_image();
        if (icon) {
            icon_size = initial_icon_size;
            if (m_stm.m_decrease_factor > 0) {
                icon_size -= m_stm.m_decrease_factor;
            }

            this->draw_icon(cr, item, icon, icon_size, area, idx, x, y);
        }

        // draw indicator
        this->draw_indicator(cr, item, area, x, y, width, height);
    }
}

inline void Panel_render::draw_cell(const Cairo::RefPtr<Cairo::Context>& cr,
                                    const shared_ptr<DockItem>& item, int x, int y, int width,
                                    int height, int area, Gtk::Orientation orientation) const
{
    if (item->get_dock_item_type() == dock_item_type_t::launcher) {
        if (m_theme.PanelCell().Fill().Color::alpha > 0.0) {
            cr->set_source_rgba(
                m_theme.PanelCell().Fill().Color::red, m_theme.PanelCell().Fill().Color::green,
                m_theme.PanelCell().Fill().Color::blue, m_theme.PanelCell().Fill().Color::alpha);

            cairo_util::rounded_rectangle(cr, x, y + 1, width, height - 2,
                                          m_theme.PanelCell().Ratio());
            cr->fill();
        }

        if (m_theme.PanelCell().Stroke().Color::alpha > 0.0) {
            cr->set_source_rgba(m_theme.PanelCell().Stroke().Color::red,
                                m_theme.PanelCell().Stroke().Color::green,
                                m_theme.PanelCell().Stroke().Color::blue,
                                m_theme.PanelCell().Stroke().Color::alpha);

            cr->set_line_width(m_theme.PanelCell().LineWidth());

            cairo_util::rounded_rectangle(cr, x, y + 1, width, height - 2,
                                          m_theme.PanelCell().Ratio());
            cr->stroke();
        }
    }
}

inline void Panel_render::draw_indicator(const Cairo::RefPtr<Cairo::Context>& cr,
                                         const shared_ptr<DockItem>& item, int area, int x, int y,
                                         int width, int height) const
{
    if (item->get_dock_item_type() != dock_item_type_t::launcher) {
        return;
    }

    if (item->m_items.size() < 1) {
        return;
    }

    auto const indicator_type = config::get_indicator_type();
    int center = (width / 2);

    if (indicator_type == dock_indicator_type_t::dots &&
        m_theme.PanelIndicator().Fill().Color::alpha > 0.0) {
        if (item->m_items.size() == 1) {
            cr->arc(x + center, y + area - 3, 1.8, 0, 2 * M_PI);
        } else if (item->m_items.size() > 1) {
            cr->arc(x + center - 3, y + area - 3, 1.8, 0, 2 * M_PI);
            cr->arc(x + center + 3, y + area - 3, 1.8, 0, 2 * M_PI);
        }

        cr->set_source_rgba(m_theme.PanelIndicator().Fill().Color::red,
                            m_theme.PanelIndicator().Fill().Color::green,
                            m_theme.PanelIndicator().Fill().Color::blue,
                            m_theme.PanelIndicator().Fill().Color::alpha);
        cr->fill();

    } else if (indicator_type == dock_indicator_type_t::lines &&
               m_theme.PanelIndicator().Stroke().Color::alpha > 0.0) {
        int marginY = area - 2;
        if (item->m_items.size() > 0) {
            if (item->m_items.size() == 1) {
                cr->move_to(x + 0, y + marginY);
                cr->line_to(x + 0, y + marginY);
                cr->line_to(x + width - 0, y + marginY);

            } else if (item->m_items.size() > 1) {
                cr->move_to(x + 0, y + marginY);
                cr->line_to(x + 0, y + marginY);
                cr->line_to(x + center - 3, y + marginY);

                cr->move_to(x + center + 3, y + marginY);
                cr->line_to(x + center + 3, y + marginY);
                cr->line_to(x + width - 0, y + marginY);
            }
        }

        cr->set_source_rgba(m_theme.PanelIndicator().Stroke().Color::red,
                            m_theme.PanelIndicator().Stroke().Color::green,
                            m_theme.PanelIndicator().Stroke().Color::blue,
                            m_theme.PanelIndicator().Stroke().Color::alpha);

        cr->set_line_width(m_theme.PanelIndicator().LineWidth());
        cr->stroke();
    }
}

inline void Panel_render::draw_active_window_indicator(const Cairo::RefPtr<Cairo::Context>& cr,
                                                       int x, int y, int idx, int width,
                                                       int height) const
{
    if (!m_stm.m_dragdrop_begin && idx > 0 && m_stm.m_active_window_index == (int)idx) {
        cairo_util::rounded_rectangle(cr, x, y, width, height, 0);
        cr->set_source_rgba(1, 1, 1, 0.2);
        cr->fill();
    }
}

inline void Panel_render::draw_drag_indicator(const Cairo::RefPtr<Cairo::Context>& cr,
                                              const shared_ptr<DockItem>& item, int x, int y,
                                              int idx, int width, int height,
                                              Gtk::Orientation orientation) const
{
    if (m_stm.m_dragdrop_begin && idx == m_drop_index) {
        if (m_theme.PanelDrag().Fill().Color::alpha > 0.0) {
            cr->set_source_rgba(
                m_theme.PanelDrag().Fill().Color::red, m_theme.PanelDrag().Fill().Color::green,
                m_theme.PanelDrag().Fill().Color::blue, m_theme.PanelDrag().Fill().Color::alpha);

            cairo_util::rounded_rectangle(cr, x, y, width, height, m_theme.PanelDrag().Ratio());
            cr->fill();
        }

        if (m_theme.PanelDrag().Stroke().Color::alpha > 0.0) {
            cr->set_source_rgba(m_theme.PanelDrag().Stroke().Color::red,
                                m_theme.PanelDrag().Stroke().Color::green,
                                m_theme.PanelDrag().Stroke().Color::blue,
                                m_theme.PanelDrag().Stroke().Color::alpha);

            cr->set_line_width(m_theme.PanelDrag().LineWidth());
            cairo_util::rounded_rectangle(cr, x, y, width, height, m_theme.PanelDrag().Ratio());
            cr->stroke();
        }
    }
}

inline void Panel_render::draw_separator(const Cairo::RefPtr<Cairo::Context>& cr,
                                         const shared_ptr<DockItem>& item, const int x, const int y,
                                         const int width, const int height,
                                         const Gtk::Orientation orientation)

{
    if (config::is_separator_line() && m_theme.PanelSeparator().Stroke().Color::alpha > 0.0 &&
        item->get_dock_item_type() == dock_item_type_t::separator) {
        cr->set_source_rgba(m_theme.PanelSeparator().Stroke().Color::red,
                            m_theme.PanelSeparator().Stroke().Color::green,
                            m_theme.PanelSeparator().Stroke().Color::blue,
                            m_theme.PanelSeparator().Stroke().Color::alpha);

        cr->set_line_width(m_theme.PanelSeparator().LineWidth());

        if (orientation == Gtk::ORIENTATION_HORIZONTAL) {
            int center = item->get_width() / 2;
            cr->move_to(x + center, y + 6);
            cr->line_to(x + center, y + 6);
            cr->line_to(x + center, y + height - 6);
        } else {
            int center = item->get_height() / 2;
            cr->move_to(x + 6, y + center);
            cr->line_to(x + 6, y + center);
            cr->line_to(x + width - 6, y + center);
        }

        cr->stroke();
    }
}
inline void Panel_render::draw_icon(const Cairo::RefPtr<Cairo::Context>& cr,
                                    const shared_ptr<DockItem>& item,
                                    const Glib::RefPtr<Gdk::Pixbuf>& icon, int icon_size, int area,
                                    int idx, int x, int y)
{
    if (icon && item->get_dock_item_type() == dock_item_type_t::launcher) {
        // reload or scaled if needed
        if (icon->get_height() != icon_size || icon->get_width() != icon_size) {
            if (idx == 0) {
                // load home icon
                load_home_icon(icon_size);
            } else {
                // Get a fresh new icon
                auto const tmp_pixbuf = pixbuf_util::get_window_icon(
                    item->get_wnckwindow(), item->get_desktop_icon_name(), icon_size);

                if (!tmp_pixbuf) {
                    //                    g_error("Load fresh %s\n", item->get_name().c_str());
                    //  auto winicon = wnck_window_get_icon(item->get_wnckwindow());
                    //  auto cicon = Glib::wrap(winicon, true);
                    item->set_image(icon->scale_simple(icon_size, icon_size, Gdk::INTERP_BILINEAR));

                } else {
                    item->set_image(
                        tmp_pixbuf->scale_simple(icon_size, icon_size, Gdk::INTERP_BILINEAR));
                }
            }
        }

        int center_pos = (area / 2) - (icon_size / 2);
        Gdk::Cairo::set_source_pixbuf(cr, item->get_image(), x + center_pos, y + center_pos);
        cr->paint();

        if (m_show_selector) {
            //  draw selector
            if ((int)idx == m_current_index && !m_context_menu_open && m_stm.m_mouse_inside &&
                (int)idx != m_inverted_index) {
                // create copy and invert image colors
                auto tmp = item->get_image()->copy();
                pixbuf_util::invert_pixels(tmp);
                Gdk::Cairo::set_source_pixbuf(cr, tmp, x + center_pos, y + center_pos);

                m_inverted_index = (int)m_current_index;
                cr->paint();

                // normal selector cell
                cr->set_source_rgba(1, 1, 1, 0.2);
                cairo_util::rounded_rectangle(cr, x, y, area, area, 0);
                cr->fill();
            }
        }
    }
}
void Panel_render::draw_title()
{
    if (m_current_index < 0 || m_context_menu_open || !config::is_show_title() ||
        !m_stm.m_connect_draw_signal_set || !m_autohide.is_visible() || m_preview) {
        m_titlewindow.hide();
        m_titlewindow_visible = false;

        return;
    }

    if (!m_stm.m_mouse_inside ||
        (m_current_index == m_title_item_index && m_title_timer.elapsed() > 4.0)) {
        m_title_timer.stop();
        m_title_timer.reset();
        m_titlewindow.hide();
        m_titlewindow_visible = false;

        return;
    }

    if (m_current_index != m_title_item_index) {
        m_title_timer.reset();
        m_title_timer.start();
        m_title_item_index = m_current_index;
        m_titlewindow.hide();
        m_titlewindow_visible = false;

        return;
    }

    // delay before show up
    if (m_title_timer.elapsed() < 0.8) {
        m_titlewindow.hide();
        m_titlewindow_visible = false;
        int y = 0, x = 0;
        position_util::get_center_position(m_current_index, x, y, 140, 26);
        m_titlewindow.move(x, y);
        return;
    }

    if (m_titlewindow_visible) {
        return;
    }

    m_titlewindow_visible = true;
    auto const item = AppUpdater::m_dockitems[m_current_index];
    int x = 0, y = 0;

    char title[60];
    int count = (int)item->m_items.size();
    if (count > 1) {
        sprintf(title, "%s (%d)", item->get_title().c_str(), count);
    } else {
        sprintf(title, "%s", item->get_title().c_str());
    }

    m_titlewindow.set_text(title);

    int height = m_titlewindow.get_height();
    int width = m_titlewindow.get_width();

    position_util::get_center_position(m_current_index, x, y, width, height);
    m_titlewindow.move(x, y);
}

inline void Panel_render::get_item_position(const dock_item_type_t item_typ, int& x, int& y,
                                            int& width, int& height)
{
    static int nextsize = 0;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        if (x == 0) {
            y = 0;

            x = config::get_window_start_end_margin() / 2;
            nextsize = width;
            return;
        }

        x += nextsize + config::get_separator_margin();
        nextsize = width;
    } else {
        if (y == 0) {
            x = 0;
            y = config::get_window_start_end_margin() / 2;
            nextsize = height;
            return;
        }

        y += nextsize + config::get_separator_margin();
        nextsize = height;
    }
}
DL_NS_END
