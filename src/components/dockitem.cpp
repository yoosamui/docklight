#include "dockitem.h"

DL_NS_BEGIN

DockItem::DockItem(appinfo_t appinfo)
{
    m_app_info = appinfo;
}
DockItem::~DockItem()
{
    g_print("Free DockItem\n");
}

string DockItem::get_name() const
{
    return m_app_info.m_name;
}

WnckWindow* DockItem::get_wnckwindow() const
{
    return m_app_info.m_wnckwindow;
}

DL_NS_END
