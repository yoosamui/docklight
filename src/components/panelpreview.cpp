#include "panelpreview.h"
#include <gdkmm/general.h>  // set_source_pixbuf()
#include "components/appupdater.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/string.h"
#include "utils/system.h"
#include "utils/wnck.h"

DL_NS_BEGIN

vector<shared_ptr<DockItem>> Panel_preview::m_previewitems;
Panel_preview::Panel_preview() : Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
{
    // Set up the top-level window.
    set_title("PanelPreview");
    set_decorated(false);
    set_app_paintable(true);

    // clang-format off
    // Set masks for mouse events
    add_events(
            Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
            Gdk::ENTER_NOTIFY_MASK |
            Gdk::LEAVE_NOTIFY_MASK |
            Gdk::POINTER_MOTION_MASK);

    // clang-format on

    GdkScreen* screen;
    GdkVisual* visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }
}

Panel_preview::~Panel_preview()
{
    for (auto item : m_previewitems) {
        auto appinfo = item->get_appinfo();
        appinfo->m_image.reset();
        item->m_preview_first_image.reset();
        item->m_preview_frame_count = 0;
        item->m_preview_image_is_dynamic = true;
    }

    m_previewitems.clear();
    g_print("Destroy preview\n");
}

Panel_preview::type_signal_close Panel_preview::signal_close()
{
    return m_signal_close;
}

void Panel_preview::init(int index)
{
    m_dockitem_index = index;

    m_theme = config::get_theme();

    // Using assignment operator to copy the items vector
    m_previewitems = AppUpdater::m_dockitems[m_dockitem_index]->m_items;

    for (auto item : m_previewitems) {
        item->get_appinfo()->m_image.reset();
        item->m_preview_first_image.reset();
        item->m_preview_frame_count = 0;
        item->m_preview_image_is_dynamic = true;
    }

    // Buble sort by name
    int size = (int)m_previewitems.size();
    int i, m, j;

    for (i = 0; i < size - 1; i = i + 1) {
        m = i;
        for (j = i + 1; j < size; j = j + 1) {
            std::string s1 = m_previewitems.at(j)->get_windowname().c_str();
            std::string s2 = m_previewitems.at(m)->get_windowname().c_str();

            s1 = string_util::string_to_lower(s1);
            s2 = string_util::string_to_lower(s2);

            if (s1 < s2) {
                m = j;
            }
        }
        std::swap(m_previewitems[i], m_previewitems[m]);
    }

    m_initialized = true;
}

bool Panel_preview::show_preview()
{
    if (!m_initialized) {
        return false;
    }

    int x = 0;
    int y = 0;
    int image_size = (m_previewitems[0]->get_width() * 2) + 24;
    int item_count = (int)m_previewitems.size();
    int separator_size = (item_count - 1) * PREVIEW_SPARATOR_SIZE;

    m_width = image_size + PREVIEW_WIDTH_EXTENDED_SIZE;
    m_height = image_size - PREVIEW_TITLE_SIZE;

    // clang-format off
    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        m_window_width = (item_count * m_width) + PREVIEW_START_END_MARGIN + separator_size;
        int max_size = position_util::get_workarea().get_width();

        if (m_window_width > max_size) {
            m_width = (max_size - PREVIEW_START_END_MARGIN - separator_size) / item_count;
            m_window_width = max_size;
            m_height = m_width - PREVIEW_WIDTH_EXTENDED_SIZE - PREVIEW_START_END_MARGIN;

            if (m_height <= PREVIEW_LIMIT_HEIGHT) {
                return false;
            }
        }

        m_window_height = m_height + PREVIEW_START_END_MARGIN + PREVIEW_TITLE_SIZE;

    } else {
        m_window_height = (item_count * m_height) + separator_size +
                          (item_count)*PREVIEW_TITLE_SIZE + PREVIEW_START_END_MARGIN;

        int max_size = position_util::get_workarea().get_height();

        if (m_window_height > max_size) {
            m_height = (max_size - PREVIEW_START_END_MARGIN - separator_size -
                        (PREVIEW_TITLE_SIZE * item_count)) /
                       item_count;

            m_window_height = max_size;
            m_width = m_height + PREVIEW_WIDTH_EXTENDED_SIZE;

            if (m_width <= PREVIEW_LIMIT_WIDTH) {
                return false;
            }
        }

        m_window_width = m_width + PREVIEW_START_END_MARGIN;
    }
    // clang-format on

    position_util::get_center_position(m_dockitem_index, x, y, m_window_width, m_window_height);

    this->resize(m_window_width, m_window_height);
    this->move(x, y);
    this->show();

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Panel_preview::on_timeout_draw), 1000 / 9);
    return true;
}

void Panel_preview::on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data)
{
}

bool Panel_preview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouse_in = false;
    return true;
}
bool Panel_preview::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouse_in = true;
    return true;
}

bool Panel_preview::on_timeout_draw()
{
    if (!m_mouse_in) {
        int mouse_x, mouse_y;
        if (system_util::get_mouse_position(mouse_x, mouse_y)) {
            int px, py;
            this->get_position(px, py);
            bool close = false;
            if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
                close = mouse_y < py;

                if (config::get_dock_location() == dock_location_t::top) {
                    close = mouse_y > this->get_height() + py;
                }

            } else {
                close = mouse_x < px;
                if (config::get_dock_location() == dock_location_t::left) {
                    close = mouse_x > this->get_width() + px;
                }
            }

            if (close) {
                m_signal_close.emit();
                return false;
            }
        }
    }

    if (m_static_count != (int)m_previewitems.size()) {
        Gtk::Widget::queue_draw();
    } else {
        if (m_old_index != m_current_index) {
            m_old_index = m_current_index;

            Gtk::Widget::queue_draw();
        }
    }

    return true;
}

bool Panel_preview::on_button_press_event(GdkEventButton* event)
{
    if (m_current_index == -1) return true;

    int mouse_x = (int)event->x;
    int mouse_y = (int)event->y;

    auto const item = m_previewitems[m_current_index];

    // check close button over
    Gdk::Rectangle mouse_rect(mouse_x, mouse_y, 2, 2);
    if (m_close_button_rectangle.intersects(mouse_rect)) {
        wnck_util::close_window(item->get_wnckwindow());

        m_previewitems.erase(m_previewitems.begin() + m_current_index);
        m_current_index = 0;

        if (m_previewitems.size() == 0) {
            this->close();
        }

        this->show_preview();

        return true;
    }

    // set image to null to force reload from cache
    if (system_util::is_mutter_window_manager() == false) {
        //        item->set_image(NULLPB);
        item->m_preview_image_is_dynamic = true;
        item->m_preview_frame_count = 0;
    }

    wnck_util::activate_window(item->get_wnckwindow());

    return true;
}

bool Panel_preview::on_button_release_event(GdkEventButton* event)
{
    return true;
}

bool Panel_preview::on_motion_notify_event(GdkEventMotion* event)
{
    int mouse_x = (int)event->x;
    int mouse_y = (int)event->y;

    auto index = this->get_index(mouse_x, mouse_y);
    m_current_index = index;

    return true;
}

inline int Panel_preview::get_index(const int& mouseX, const int& mouseY)
{
    Gdk::Point mouse(mouseX, mouseY);

    int idx = 0;
    int x = PREVIEW_START_END_MARGIN / 2;
    int y = x;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        for (auto item : m_previewitems) {
            if (mouse.get_x() >= x && mouse.get_x() <= x + m_width) {
                return idx;
            }

            x += m_width + PREVIEW_SPARATOR_SIZE;
            idx++;
        }
    } else {
        for (auto item : m_previewitems) {
            if (mouse.get_y() >= y && mouse.get_y() <= y + m_height + PREVIEW_TITLE_SIZE) {
                return idx;
            }

            y += m_height + PREVIEW_SPARATOR_SIZE + PREVIEW_TITLE_SIZE;
            idx++;
        }
    }

    return -1;
}

inline void Panel_preview::draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y,
                                     const string& text)
{
    cr->rectangle(x, y + 6, m_width - 6, PREVIEW_TITLE_SIZE);
    cr->set_source_rgba(1, 1, 1, 0.f);  // for debuging set alpha to 1.f

    cr->clip_preserve();
    cr->stroke();

    //    cr->set_source_rgba(1, 1, 1, 1.f);  // for debuging set alpha to 1.f
    if (m_theme.PreviewTitleText().Stroke().Color::alpha != 0.0) {
        cr->set_source_rgba(m_theme.PreviewTitleText().Stroke().Color::red,
                            m_theme.PreviewTitleText().Stroke().Color::green,
                            m_theme.PreviewTitleText().Stroke().Color::blue,
                            m_theme.PreviewTitleText().Stroke().Color::alpha);

        cr->stroke();
    }
    // http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
    Pango::FontDescription font;

    font.set_family("Monospace");
    font.set_weight(Pango::WEIGHT_NORMAL);
    font.set_size(9 * PANGO_SCALE);

    // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
    auto layout = create_pango_layout(text);

    layout->set_font_description(font);

    int text_width;
    int text_height;

    // get the text dimensions (it updates the variables -- by reference)
    layout->get_pixel_size(text_width, text_height);

    // Position the text in the middle
    // cr->move_to((rectangle_width-text_width)/2, (rectangle_height-text_height)/2);
    cr->move_to(x + 4, y + 4);

    layout->show_in_cairo_context(cr);
    cr->reset_clip();  // Reset the clipping
}

bool Panel_preview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    // background
    Gdk::Rectangle rect(0, 0, m_window_width, m_window_height);
    cairo_util::fill(cr, m_theme.Preview(), m_theme.PreviewGradient(), rect);
    cairo_util::stroke(cr, m_theme.Preview(), rect);

    int image_center_x = 0;
    int image_center_y = 0;

    int idx = 0;
    int x = PREVIEW_START_END_MARGIN / 2;
    int y = x;

    for (auto const item : m_previewitems) {
        auto image = item->get_image();
        GdkPixbuf* win_pixbuf = nullptr;

        if (!image || item->m_preview_image_is_dynamic) {
            if (system_util::is_mutter_window_manager() == false) {
                auto wnckwindow = item->get_wnckwindow();
                bool in_current_desktop = wnck_util::is_window_on_current_desktop(wnckwindow);
                if (!in_current_desktop || wnck_window_is_minimized(wnckwindow)) {
                    win_pixbuf = AppUpdater::get_image_from_cache(item->get_xid());
                }
            }

            if (!win_pixbuf) {
                win_pixbuf = pixbuf_util::get_gdk_pixbuf_from_window(item->get_xid());
            }

            if (win_pixbuf) {
                auto scaled_pixbuf =
                    pixbuf_util::get_gdk_pixbuf_scaled(win_pixbuf, m_width, m_height);
                //                    gdk_pixbuf_scale_simple(win_pixbuf, m_width, m_height,
                //                    GDK_INTERP_BILINEAR);

                g_object_unref(win_pixbuf);
                image = Glib::wrap(scaled_pixbuf, true);
                g_object_unref(scaled_pixbuf);

                if (image) {
                    if (item->m_preview_frame_count == 0) {
                        item->m_preview_first_image = image;
                    }

                    item->set_image(image);
                }
            }
        }

        if (item->m_preview_frame_count != -1 && item->m_preview_image_is_dynamic &&
            ++item->m_preview_frame_count > 4) {
            // g_print("%d\n", pixbuf_util::compare_pixels(item->m_preview_first_image, image,
            // false));

            if (pixbuf_util::compare_pixels(item->m_preview_first_image, image, false) == 1) {
                g_print("static detected: %s %d\n", item->get_title().c_str(),
                        (int)item->get_xid());
                item->m_preview_image_is_dynamic = false;
                m_static_count++;
            }

            item->m_preview_frame_count = -1;
        }

        // Draw cell
        // clang-format off
        if (m_theme.PreviewCell().Fill().Color::alpha != 0.0) {

            cr->set_source_rgba(m_theme.PreviewCell().Fill().Color::red,
                    m_theme.PreviewCell().Fill().Color::green,
                    m_theme.PreviewCell().Fill().Color::blue,
                    m_theme.PreviewCell().Fill().Color::alpha);

            cairo_util::rounded_rectangle(cr, x, y, m_width, m_height + PREVIEW_TITLE_SIZE, m_theme.PreviewCell().Ratio());
            cr->fill();
        }
        if (m_theme.PreviewCell().Stroke().Color::alpha != 0.0) {

            cr->set_source_rgba(m_theme.PreviewCell().Stroke().Color::red,
                    m_theme.PreviewCell().Stroke().Color::green,
                    m_theme.PreviewCell().Stroke().Color::blue,
                    m_theme.PreviewCell().Stroke().Color::alpha);


            cr->set_line_width(m_theme.PreviewCell().LineWidth());
            cairo_util::rounded_rectangle(cr, x, y, m_width, m_height + PREVIEW_TITLE_SIZE, m_theme.PreviewCell().Ratio());
            cr->stroke();
        }

        // title selector
        if (idx == m_current_index) {
            if (m_theme.PreviewTitleText().Fill().Color::alpha != 0.0) {

                cr->set_source_rgba(m_theme.PreviewTitleText().Fill().Color::red,
                        m_theme.PreviewTitleText().Fill().Color::green,
                        m_theme.PreviewTitleText().Fill().Color::blue,
                        m_theme.PreviewTitleText().Fill().Color::alpha);

                cairo_util::rounded_rectangle(cr, x+4, y+2, m_width-6, PREVIEW_TITLE_SIZE, 0);
                cr->fill();
            }
        }
        // clang-format on

        // cell selector
        if (idx == m_current_index) {
            if (m_theme.PreviewTitleText().Fill().Color::alpha != 0.0) {
                cr->set_source_rgba(1, 1, 1, 0.4);

                cairo_util::rounded_rectangle(cr, x, y, m_width, m_height + PREVIEW_TITLE_SIZE,
                                              m_theme.PreviewCell().Ratio());
                cr->fill();
            }
        }
        // Image and text
        if (image) {
            image_center_x = (m_width / 2) - (image->get_width() / 2);
            image_center_y = ((m_height / 2) - (image->get_height() / 2)) + PREVIEW_TITLE_SIZE;

            draw_text(cr, x, y, item->get_windowname());

            // close selector
            if (idx == m_current_index) {
                int x1 = x + m_width - PREVIEW_TITLE_SIZE - 2;
                int x2 = PREVIEW_TITLE_SIZE - 10;
                int y1 = y + 1;
                int y2 = PREVIEW_TITLE_SIZE - 10;

                m_close_button_rectangle =
                    Gdk::Rectangle(x1, y1, PREVIEW_TITLE_SIZE, PREVIEW_TITLE_SIZE);

                if (m_theme.PreviewClose().Fill().Color::alpha != 0.0) {
                    cr->set_source_rgba(m_theme.PreviewClose().Fill().Color::red,
                                        m_theme.PreviewClose().Fill().Color::green,
                                        m_theme.PreviewClose().Fill().Color::blue,
                                        m_theme.PreviewClose().Fill().Color::alpha);

                    cairo_util::rounded_rectangle(cr, x1, y1, PREVIEW_TITLE_SIZE,
                                                  PREVIEW_TITLE_SIZE,
                                                  m_theme.PreviewClose().Ratio());
                    cr->fill();
                }

                if (m_theme.PreviewClose().Stroke().Color::alpha != 0.0) {
                    cr->set_source_rgba(m_theme.PreviewClose().Stroke().Color::red,
                                        m_theme.PreviewClose().Stroke().Color::green,
                                        m_theme.PreviewClose().Stroke().Color::blue,
                                        m_theme.PreviewClose().Stroke().Color::alpha);

                    cr->set_line_width(m_theme.PreviewClose().LineWidth());

                    x1 += 5;
                    y1 += 5;

                    cr->move_to(x1, y1);
                    cr->line_to(x1, y1);
                    cr->line_to(x1 + x2, y1 + y2);
                    cr->stroke();

                    cr->move_to(x1 + x2, y1);
                    cr->line_to(x1 + x2, y1);
                    cr->line_to(x1, y1 + y2);

                    cr->stroke();
                }
            }

            Gdk::Cairo::set_source_pixbuf(cr, item->get_image(), x + image_center_x,
                                          y + image_center_y);
            cr->paint();
        }

        if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
            x += m_width + PREVIEW_SPARATOR_SIZE;
        } else {
            y += m_height + PREVIEW_SPARATOR_SIZE + PREVIEW_TITLE_SIZE;
        }

        idx++;
    }

    return true;
}

DL_NS_END

