//*****************************************************************
//
//  Copyright © 2018 Juan R. González
//  j-gonzalez@email.de
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//*****************************************************************


#include "DockItem.h"
#include "Configuration.h"
#include "DockPanel.h"

DockItem::DockItem():
    DockItem(Configuration::get_CellWidth(), Configuration::get_CellHeight())
{
}

DockItem::DockItem(const guint width, const guint height)
{
    this->m_width = width == 0 ? Configuration::get_CellWidth() : width;
    this->m_height = height == 0 ? Configuration::get_CellHeight() : height;
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
    if (DockPanel::get_WidthDecrement() < 0 ){
        return this->m_width;
    }

    return this->m_width - DockPanel::get_WidthDecrement();
}


guint DockItem::get_Height()
{
    if (DockPanel::get_HeightDecrement() < 0 ){
        return this->m_height;
    }

    return this->m_height - DockPanel::get_HeightDecrement();
}


guint DockItem::get_InmutableWidth()
{
    return this->m_width;
}

guint DockItem::get_InmutableHeight()
{
    return this->m_height;
}


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




