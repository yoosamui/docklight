#include "dockitem.h"
#include "components/config.h"
#include "components/panel.h"

//#include "utils/position.h"

DL_NS_BEGIN

DockItem::DockItem(appinfo_t app_info, dock_item_type_t item_type)
{
    m_app_info = app_info;
    m_app_info.m_dock_item_type = item_type;
    m_app_info.m_width = m_app_info.m_height = config::get_dock_area();

    if (m_app_info.m_dock_item_type == dock_item_type_t::separator) {
        this->swap_size();
    }
}

DockItem::~DockItem()
{
    // if (m_app_info.m_image) {
    // m_app_info.m_image.reset();
    // m_app_info.m_image = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;

    // g_print("Free DockItem image\n");
    //}
    // g_free(&m_app_info);
    g_print("Free DockItem\n");
}

appinfo_t* DockItem::get_appinfo()
{
    return &m_app_info;
}

void DockItem::set_attach(bool attach)
{
    m_attached = attach;
}

dock_item_type_t DockItem::get_dock_item_type() const
{
    return m_app_info.m_dock_item_type;
}

string DockItem::get_name() const
{
    return m_app_info.m_name;
}

string DockItem::get_title() const
{
    return m_app_info.m_title;
}

string DockItem::get_windowname() const
{
    if (!WNCK_IS_WINDOW(m_app_info.m_wnckwindow)) {
        return "invalid window";
    }

    return wnck_window_get_name(m_app_info.m_wnckwindow);
}

string DockItem::get_desktop_filename() const
{
    return m_app_info.m_desktop_file;
}

void DockItem::set_desktop_filename(const string& desktop_file)
{
    m_app_info.m_desktop_file = desktop_file;
}

string DockItem::get_desktop_icon_name() const
{
    return m_app_info.m_icon_name;
}

WnckWindow* DockItem::get_wnckwindow() const
{
    return m_app_info.m_wnckwindow;
}

unsigned long DockItem::get_xid() const
{
    return m_app_info.m_xid;
}
void DockItem::set_image(Glib::RefPtr<Gdk::Pixbuf> image)
{
    m_app_info.m_image = image;
}

const Glib::RefPtr<Gdk::Pixbuf> DockItem::get_image()
{
    return m_app_info.m_image;
}

bool DockItem::is_attached() const
{
    return m_attached;
}

void DockItem::set_expander_size(int size)
{
    if (m_app_info.m_dock_item_type != dock_item_type_t::expander) {
        return;
    }

    int area = config::get_dock_area();
    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        m_app_info.m_height = area;
        m_app_info.m_width = size;
    } else {
        m_app_info.m_width = area;
        m_app_info.m_height = size;
    }
}

void DockItem::swap_size()
{
    if (m_app_info.m_dock_item_type != dock_item_type_t::separator) {
        return;
    }

    int area = config::get_dock_area();
    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        m_app_info.m_height = area;
        m_app_info.m_width = config::get_separator_size();
    } else {
        m_app_info.m_width = area;
        m_app_info.m_height = config::get_separator_size();
    }
}

int DockItem::get_width() const
{
    int size = m_app_info.m_width - Panel::m_stm.m_decrease_factor;
    if (size < 1) {
        size = 1;
    }

    return size;
}

int DockItem::get_height() const
{
    int size = m_app_info.m_height - Panel::m_stm.m_decrease_factor;
    if (size < 1) {
        size = 1;
    }
    return size;
}

void DockItem::set_width(int value)
{
    m_app_info.m_width = value;
}

void DockItem::set_height(int value)
{
    m_app_info.m_height = value;
}
void DockItem::set_index(int index)
{
    m_index = index;
}

int DockItem::get_index() const
{
    return m_index;
}
void DockItem::set_y(int y)
{
    m_y = y;
}

void DockItem::set_x(int x)
{
    m_x = x;
}

int DockItem::get_y()
{
    return m_y;
}

int DockItem::get_x()
{
    return m_x;
}

shared_ptr<DockItem> DockItem::get_next() const
{
    static int index = 0;

    if (m_items.size() == 0) return shared_ptr<DockItem>(nullptr);

    if (index >= (int)m_items.size()) index = 0;

    auto result = m_items[index];
    index++;

    return result;
}
DL_NS_END
