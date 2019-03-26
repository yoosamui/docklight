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

    int getStartPos(const DockItem& item, const int count, const int index, const int width)
    {

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


    }

    int getCenter(int count, int index, int width)
    {

        int monitorWidth = DockWindow::get_geometry().width;
        int monitorX = DockWindow::get_geometry().x;
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

