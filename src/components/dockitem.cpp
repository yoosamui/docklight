#include "dockitem.h"

DL_NS_BEGIN

DockItem::DockItem(appinfo_t appinfo)
{
    m_app_info = appinfo;
    m_height = 48;
    m_width = 48;
}
DockItem::~DockItem()
{
    g_print("Free DockItem\n");
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

unsigned int DockItem::get_width() const
{
    return m_width;
}

unsigned int DockItem::get_height() const
{
    return m_height;
}

void DockItem::set_index(unsigned int index)
{
    m_index = index;
}

void DockItem::set_y(unsigned int y)
{
    m_y = y;
}

void DockItem::set_x(unsigned int x)
{
    m_x = x;
}

DL_NS_END
