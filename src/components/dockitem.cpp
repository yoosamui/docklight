#include "dockitem.h"
#include "components/config.h"
#include "components/panel.h"

DL_NS_BEGIN

DockItem::DockItem(appinfo_t app_info, dock_item_type_t item_type)
{
    m_app_info = app_info;
    m_app_info.m_dock_item_type = item_type;
    m_app_info.m_width = m_app_info.m_height = config::get_dock_area();
}

DockItem::~DockItem()
{
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

void DockItem::swap_size()
{
    int area = config::get_dock_area();

    if (config::get_dock_orientation() == Gtk::ORIENTATION_HORIZONTAL) {
        m_app_info.m_height = area;
        m_app_info.m_width = m_app_info.m_separator_length;
    } else {
        m_app_info.m_width = area;
        m_app_info.m_height = m_app_info.m_separator_length;
    }
}

int DockItem::get_width()
{
    if (m_app_info.m_dock_item_type == dock_item_type_t::separator) {
        this->swap_size();
        return m_app_info.m_width;  // - Panel::m_stm.m_decrease_factor;
    }

    return m_app_info.m_width - Panel::m_stm.m_decrease_factor;
}

int DockItem::get_height()
{
    if (m_app_info.m_dock_item_type == dock_item_type_t::separator) {
        this->swap_size();
        return m_app_info.m_height;  // - Panel::m_stm.m_decrease_factor;
    }

    return m_app_info.m_height - Panel::m_stm.m_decrease_factor;
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
