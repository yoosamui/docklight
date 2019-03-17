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
    this->m_image_draw = true; //// NOT USED
}

DockItem::DockItem(const DockItem& orig) {
}

DockItem::~DockItem() {
}

