/* 
 * File:   DockItem.cpp
 * Author: yoo
 * 
 * Created on November 11, 2018, 5:14 PM
 */

#include "DockItem.h"
#include "Configuration.h"
DockItem::DockItem() {
    
    this->m_width = Configuration::get_CellWidth();
    this->m_height = Configuration::get_CellHeight();
}

DockItem::~DockItem() {
}

/**
 * get the current item from the items vector
 * @return DockItem*
 */
DockItem* DockItem::GetCurrent()
{
    if (m_items.size() == 0)
        return nullptr;

    if (m_index >= (int) m_items.size())
        m_index = 0;

    DockItem* result = m_items.at(m_index);

    return result;

}

/**
 * get the next item from the items vector
 * @return DockItem*
 */
DockItem* DockItem::GetNext()
{
    if (m_items.size() == 0)
        return nullptr;

    if (m_index >= (int) m_items.size())
        m_index = 0;

    DockItem* result = m_items.at(m_index);
    m_index++;

    return result;

}

std::string DockItem::get_Title()
{
    if (!m_titlename.empty())
        return m_titlename;

    return m_realgroupname;
}

std::string DockItem::getGroupName()
{
    std::replace(m_realgroupname.begin(), 
            m_realgroupname.end(), ' ', '-');

    return m_realgroupname;
}

std::string DockItem::getDesktopFileName()
{
  return getGroupName() + ".desktop";
}



