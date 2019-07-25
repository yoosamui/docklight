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

dock_item_type_t DockItem::get_dock_item_type() const
{
    return m_app_info.m_dock_item_type;
}

string DockItem::get_name() const
{
    return m_app_info.m_name;
}

string DockItem::get_desktop_icon_name() const
{
    return m_app_info.m_desktop_icon_name;
}

WnckWindow* DockItem::get_wnckwindow() const
{
    return m_app_info.m_wnckwindow;
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

void DockItem::set_y(int y)
{
    m_y = y;
}

void DockItem::set_x(int x)
{
    m_x = x;
}

DL_NS_END
