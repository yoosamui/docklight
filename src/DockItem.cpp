/* 
 * File:   DockItem.cpp
 * Author: yoo
 * 
 * Created on November 11, 2018, 5:14 PM
 */

#include "DockItem.h"
#include "Configuration.h"
DockItem::DockItem() {
    this->m_width = DEF_CELLWIDTH;
    this->m_height = DEF_CELLHIGHT;//Configuration::get_itemSize();
}

DockItem::DockItem(const DockItem& orig) {
}

DockItem::~DockItem() {
}

