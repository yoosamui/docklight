#include "dockitem.h"
#include "components/config.h"
DL_NS_BEGIN

DockItem::DockItem(appinfo_t app_info, dock_item_type_t item_type)
{
    m_app_info = app_info;
    m_app_info.m_dock_item_type = item_type;

    m_width = m_height = config::get_icon_size();
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

string DockItem::get_desktop_icon_name() const
{
    return m_app_info.m_desktop_icon_name;
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

int DockItem::get_width() const
{
    return m_width;
}

int DockItem::get_height() const
{
    return m_height;
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
