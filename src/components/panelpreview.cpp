#include "panelpreview.h"
#include "components/appupdater.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/system.h"
#include "utils/wnck.h"

#include <gdkmm/general.h>  // set_source_pixbuf()
DL_NS_BEGIN

// vector<shared_ptr<DockItem>> Panel_preview::m_previewitems;
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

    WnckScreen* wnckscreen = wnck_screen_get_default();
    g_signal_connect(wnckscreen, "active_window_changed",
                     G_CALLBACK(Panel_preview::on_active_window_changed), nullptr);

    // m_font.set_family("System");
    // m_font.set_size(8 * PANGO_SCALE);
    // m_font.set_weight(Pango::WEIGHT_NORMAL);
}

Panel_preview::~Panel_preview()
{
    // for (auto item : AppUpdater::m_dockitems[m_current_index]->m_items) {
    // item->m_preview_first_image = NULLPB;
    // item->set_image(NULLPB);
    // item.reset();
    //}

    // m_previewitems.clear();
    g_print("Destroy preview\n");
}

Panel_preview::type_signal_close Panel_preview::signal_close()
{
    return m_signal_close;
}

bool initialized = false;
void Panel_preview::init(int index)
{
    m_current_index = index;

    // Using assignment operator to copy the items vector
    m_previewitems = AppUpdater::m_dockitems[m_current_index]->m_items;

    ////    auto const items = AppUpdater::m_dockitems[index];
    for (auto item : m_previewitems) {
        item->set_image(NULLPB);
        item->m_preview_first_image = NULLPB;
        item->m_preview_frame_count = 0;
        item->m_preview_image_is_dynamic = true;
    }

    //    vect = items;  // AppUpdater::m_dockitems[index];

    // Using assignment operator to copy the items vector
    //  m_previewitems.insert(m_previewitems.begin(), items.begin(), items.end());

    //
    //
    initialized = true;
}

#define PREVIEW_WIDTH_EXTENDED_SIZE 60
#define PREVIEW_SPARATOR_SIZE 8
#define PREVIEW_START_END_MARGIN 20
#define PREVIEW_TITLE_SIZE 32

bool Panel_preview::show_preview()
{
    if (!initialized) {
        return false;
    }

    int x = 0;
    int y = 0;
    int image_size = m_previewitems[0]->get_width() * 3;

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        m_width = image_size + PREVIEW_WIDTH_EXTENDED_SIZE;
        m_height = image_size - PREVIEW_TITLE_SIZE;

        m_window_width = (m_previewitems.size() * m_width) + PREVIEW_START_END_MARGIN +
                         ((m_previewitems.size() - 1) * PREVIEW_SPARATOR_SIZE);
        m_window_height = image_size + PREVIEW_START_END_MARGIN;
    } else {
        m_width = image_size;
        m_height = image_size * 10;

        m_window_width = (m_previewitems.size() * m_width) + 20;
        m_window_height = m_width + 10;
    }

    this->resize(m_window_width, m_window_height);

    position_util::get_center_position(m_current_index, x, y, m_window_width, m_window_height);
    this->move(x, y);

    this->show();

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Panel_preview::on_timeout_draw), 1000 / 9);
    return true;
}
void Panel_preview::on_active_window_changed(WnckScreen* screen,
                                             WnckWindow* previously_active_window,
                                             gpointer user_data)
{
    //  WnckWindow* active_window = wnck_screen_get_active_window(screen);
    //    if (!active_window) {
    //        return;
    //    }

    //    close_this = true;
}

bool Panel_preview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouse_in = false;
    return true;
}
bool Panel_preview::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    //
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

    auto const item = m_previewitems[m_current_index];

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
    auto index = this->get_index(event->x, event->y);
    if (index != -1) m_current_index = index;

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
            if (mouse.get_y() >= y && mouse.get_y() <= y + m_height) {
                return idx;
            }

            y += m_height + PREVIEW_SPARATOR_SIZE;
            idx++;
        }
    }

    return -1;
}
void Panel_preview::draw_text(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y,
                              const string& text)
{
    cr->rectangle(x, y + 6, m_width - 6, PREVIEW_TITLE_SIZE);
    cr->set_source_rgba(1, 1, 1, 0.f);  // for debuging set alpha to 1.f

    cr->clip_preserve();
    cr->stroke();

    cr->set_source_rgba(1, 1, 1, 1.f);  // for debuging set alpha to 1.f
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
    cr->move_to(x + 4, y + 16);

    layout->show_in_cairo_context(cr);
    cr->reset_clip();  // Reset the clipping
}

bool Panel_preview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    cr->set_source_rgba(0, 0, 0, 1);
    cairo_util::rounded_rectangle(cr, 0, 0, m_window_width, m_window_height, 0);
    cr->fill();

    int idx = 0;
    int x = PREVIEW_START_END_MARGIN / 2;
    int y = x + PREVIEW_TITLE_SIZE;

    for (auto const item : m_previewitems) {
        auto image = item->get_image();

        if (!image || item->m_preview_image_is_dynamic) {
            Glib::RefPtr<Gdk::Pixbuf> win_pixbuf = NULLPB;

            if (system_util::is_mutter_window_manager() == false) {
                auto wnckwindow = item->get_wnckwindow();
                bool in_current_desktop = wnck_util::is_window_on_current_desktop(wnckwindow);
                if (!in_current_desktop ||
                    (in_current_desktop && wnck_window_is_minimized(wnckwindow))) {
                    win_pixbuf = item->m_preview_window_image;
                }
            }

            if (!win_pixbuf) {
                win_pixbuf = pixbuf_util::get_pixbuf_from_window(item->get_xid());
            }

            if (win_pixbuf) {
                guint scaled_width = 0;
                guint scaled_height = 0;

                if (system_util::is_mutter_window_manager() == false) {
                    item->m_preview_window_image = win_pixbuf;
                }

                image = pixbuf_util::get_pixbuf_scaled(win_pixbuf, m_width, m_height, scaled_width,
                                                       scaled_height);

                if (item->m_preview_frame_count == 0) {
                    item->m_preview_first_image = image;
                }

                item->set_image(image);
            }
        }

        if (item->m_preview_frame_count != -1 && item->m_preview_image_is_dynamic &&
            ++item->m_preview_frame_count > 9) {
            // return 1 if the pixbuf data contains diferences, or 0 if the pixels are equal
            // otherwise -1.
            if (pixbuf_util::compare_pixels(item->m_preview_first_image, image) == 0) {
                g_print("static %s %d\n", item->get_title().c_str(), (int)item->get_xid());
                item->m_preview_image_is_dynamic = false;
                m_static_count++;
            }

            item->m_preview_frame_count = -1;
        }

        if (idx == m_current_index) {
            cr->set_source_rgba(1, 1, 1, 0.4);
            cairo_util::rounded_rectangle(cr, x, 6, m_width, PREVIEW_TITLE_SIZE, 0);
            cr->fill();
        }
        cr->set_source_rgba(1, 1, 1, 0.4);
        cairo_util::rounded_rectangle(cr, x, y, m_width, m_height, 0);
        cr->fill();
        if (image) {
            draw_text(cr, x, 1, item->get_windowname());

            Gdk::Cairo::set_source_pixbuf(cr, image, x, y);
            cr->paint();
        }

        x += m_width + PREVIEW_SPARATOR_SIZE;
        idx++;
    }

    return true;
}

DL_NS_END

