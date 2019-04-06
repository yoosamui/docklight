/* 
 * File:   DockItemPositions.cpp
 * Author: yoo
 * 
 * Created on March 24, 2019, 12:59 PM
 */

#include "DockItemPositions.h"
#include "DockWindow.h"
#include "AppUpdater.h"

namespace DockItemPositions
{
    /**
     * Compute the width of all items.
     * The result width will resize the dockwindow width.
     * referenced by DockWindow.
     * @return The width of the dockwindow.
     */
    guint get_dockItemsWidth()
    {
        guint separatorMargin = Configuration::get_separatorMargin();
        guint size = DockWindow::get_dockWindowStartEndMargin();

        for (auto item : AppUpdater::m_dockitems) {
            size += item->m_width + Configuration::get_separatorMargin();
        }

        return size - separatorMargin;
    }

    /**
     * Compute the height of all items
     * The result width will resize the dockwindow height.
     * referenced by DockWindow.
     * @return the height of the dockwindow.
     */
    guint get_dockItemsHeight()
    {
        guint separatorMargin = Configuration::get_separatorMargin();
        guint size = DockWindow::get_dockWindowStartEndMargin();

        for (auto item : AppUpdater::m_dockitems) {
            if (item->m_dockitemtype == DockItemType::Separator){
                size += item->m_width + separatorMargin;
                continue;
            }

            size += item->m_height + separatorMargin;
        }

        return size - separatorMargin;
    }















    int getItemsSizeUntilIndex(const int index)
    {
        unsigned int size = 0;
        int count = 0;
        for (DockItem* item:AppUpdater::m_dockitems) {
            if (count == index) {
                break;
            }

            if (Configuration::get_dockWindowLocation() == panel_locationType::LEFT ||
                    Configuration::get_dockWindowLocation() == panel_locationType::RIGHT) {
                if (item->m_dockitemtype == DockItemType::Separator) {
                    size += item->m_width;
                    count++;
                    continue;
                }
                size += item->m_height;
            }
            else {

                size += item->m_width;
                g_print("--------%d---%d\n", count, item->m_width);
            }

            count++;
        }

        return size + (Configuration::get_separatorMargin() * index);
    }

    int getItemsWidth()
    {
        unsigned int size = 0;
        for (DockItem* item:AppUpdater::m_dockitems) {
            size += item->m_width;
        }

        return size + (Configuration::get_separatorMargin() * AppUpdater::m_dockitems.size());
    }

    /*
     * returns the start position from the given item.
     * The position is the absolute position from the current monitor.
     * @param: DockItem, x , y and the  width of the source window.
     * @references: the start position. x and y. Depending of the DockWindow location.
     */
    bool get_CenterPosition(const int index, int& x, int& y, const guint width, const guint height)
    {
        if (index < 0 || index > AppUpdater::m_dockitems.size()){
            return false;
        }

        DockItem* item = AppUpdater::m_dockitems[index];
        DockWindow::get_DockWindowPosition(x, y);

        if (DockWindow::is_Horizontal()){
            x += DockWindow::get_dockWindowStartEndMargin() / 2;
            if (Configuration::get_dockWindowLocation() == panel_locationType::BOTTOM){
                y -= height + 2;
            }
            else{
                y += DockWindow::get_DockWindowHeight() + 2;
            }

            for (DockItem* citem : AppUpdater::m_dockitems){
                if( citem->m_index == item->m_index){
                    x -= (width / 2) - citem->m_width  / 2;
                    return true;
                }

                x += citem->m_width + Configuration::get_separatorMargin();
            }
        }
        else{
            y += DockWindow::get_dockWindowStartEndMargin() / 2;
            if (Configuration::get_dockWindowLocation() == panel_locationType::RIGHT){
                x -= width + 2;
            }
            else{
                x += DockWindow::get_DockWindowWidth() + 2;
            }

            for (DockItem* citem : AppUpdater::m_dockitems){
                if( citem->m_index == item->m_index){
                    g_print("index %d\n", item->m_index);
                    y -= (height / 2) - citem->m_height  / 2;
                    return true;
                }
                
                if (citem->m_dockitemtype == DockItemType::Separator){
                    y += citem->m_width + Configuration::get_separatorMargin();
                }
                else {
                    y += citem->m_height + Configuration::get_separatorMargin();
                }
            }
        }

return true;

// calculate center

        

/*
        int x, y;
        int center = 0;
        DockWindow::get_DockWindowPosition(x, y);

        if (Configuration::is_panelMode() && Configuration::get_HorizontalAlignment() == Horizontal_alignment_type::CENTER) {
            center = (DockWindow::get_DockWindowWidth() / 2) - (getItemsWidth() / 2) - item.m_width / 2;
        }

        int size = getItemsSizeUntilIndex(index);
        int position = x + DockWindow::get_dockWindowStartEndMargin() / 2 +
            Configuration::get_separatorMargin() / 2 + size -
            (width / 2 - item.m_width / 2);

        // g_print("width %d\n", width);
        return position + center;
*/

       
    }

    int getCenter(int count, int index, int width)
    {
        int monitorWidth = DockWindow::Monitor::get_geometry().width;
        int monitorX = DockWindow::Monitor::get_geometry().x;
        int monitorcenter = monitorWidth / 2;
        int intemcenter = (count * Configuration::get_CellWidth()) / 2;
        int firstItempos = monitorX + (monitorcenter - intemcenter);
        int lastItempos = monitorX + (monitorcenter + (intemcenter - Configuration::get_CellWidth()));
        int cellleftpos = monitorX + firstItempos + (Configuration::get_CellWidth() * index);
        int centerpos = cellleftpos - ((width / 2) -(Configuration::get_CellWidth() / 2)) - monitorX;


        // checks left monitor limit
        if (centerpos < monitorX)
            centerpos = firstItempos;

        // checks right monitor limit
        if ((centerpos - monitorX) + width > monitorWidth)
            centerpos = (lastItempos - width) + Configuration::get_CellWidth();

        // centerpos < 0  then center on screen
        if (centerpos < 0) {
            centerpos = monitorcenter - (width / 2);

        }

        return centerpos;
    }

}

