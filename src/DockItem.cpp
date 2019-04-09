/* 
 * File:   DockItem.cpp
 * Author: yoo
 * 
 * Created on November 11, 2018, 5:14 PM
 */

#include "DockItem.h"
#include "Configuration.h"
#include "DockPanel.h"

DockItem::DockItem() {
    
    this->m_width = Configuration::get_CellWidth();
    this->m_height = Configuration::get_CellHeight();
}

/**
 * Destructor
 */
DockItem::~DockItem() 
{
    for (int i = this->m_items.size() - 1; i >= 0; i--){
        delete m_items[i];
        m_items[i] = NULL;
        m_items.erase(m_items.begin() + i);
    }

    g_print("DockItem destroy.\n");
}

guint DockItem::get_Width()
{
    return this->m_width - DockPanel::get_LimitDecrementX();
}
guint DockItem::get_Height()
{
    return this->m_height - DockPanel::get_LimitDecrementY();
}
 guint DockItem::get_InmutableWidth()
 {
    return this->m_width;
 }

guint DockItem::get_InmutableHeight()
{
    return this->m_height;
}

/*    
void DockItem::set_Width(guint value)
{
   this->m_width = value;
}

void DockItem::set_Height(guint value)
{
   this->m_height = value;

}
*/


/**
 * get the current item from the items vector
 * @return DockItem*
 */
//piece of shit is obsolete
/*
DockItem* DockItem::get_Current()
{
    if (m_items.size() == 0)
        return nullptr;

    if (m_index >= (int) m_items.size())
        m_index = 0;

    DockItem* result = m_items[m_index];

    return result;

}
*/

/**
 * get the next item from the items vector
 * @return DockItem*
 */
DockItem* DockItem::get_Next()
{
    static int index = 0;

    if (m_items.size() == 0)
        return nullptr;

    if (index >= (int) m_items.size())
        index  = 0;

    DockItem* result = m_items[index];
    index++;

    return result;

}

std::string DockItem::get_Title()
{
    if (!m_titlename.empty())
        return m_titlename;

    return m_realgroupname;
}

std::string DockItem::get_GroupName()
{
    std::replace(m_realgroupname.begin(), 
            m_realgroupname.end(), ' ', '-');

    return m_realgroupname;
}

std::string DockItem::get_DesktopFileName()
{
  return get_GroupName() + ".desktop";
}



