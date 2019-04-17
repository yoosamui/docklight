/* File:   DockItemPositions.cpp
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
     *  calculates the height size of all items. Returns the decrement value to resize the items.
     */
    guint get_ResizeHeightDecrement()
    {
        int diff = (DockWindow::Monitor::get_geometry().height/2) - get_inmutableItemsHeight();
        if (diff < 0 ){
            return (guint) abs(diff) / AppUpdater::m_dockitems.size();
        }

        return 0;
    }

    /**
     *  calculates the width size of all items. Returns the decrement value to resize the items.
     */
    guint get_ResizeWidthDecrement()
    {
        int diff = (DockWindow::Monitor::get_geometry().width/2) - get_inmutableItemsWidth();
        if (diff < 0 ){
            return (guint) abs(diff) / AppUpdater::m_dockitems.size();
        }

        return 0;
    }
    /**
     * Compute the original width of all items.
     * @return The width of all items.
     */
    guint get_inmutableItemsWidth()
    {
        guint separatorMargin = Configuration::get_inmutableSeparatorMargin();
        guint size = DockWindow::get_dockWindowStartEndMargin();

        for (auto item : AppUpdater::m_dockitems) {
            size += item->get_InmutableWidth() + separatorMargin;
        }

        return size - separatorMargin;
    }

    /**
     * Compute the original height of all items.
     * @return The height of all items.
     */
    guint get_inmutableItemsHeight()
    {
        guint separatorMargin = Configuration::get_inmutableSeparatorMargin();
        guint size = DockWindow::get_dockWindowStartEndMargin();

        for (auto item : AppUpdater::m_dockitems) {
            if (item->m_dockitemtype == DockItemType::Separator){
                size += item->get_InmutableWidth() + separatorMargin;
                continue;
            }

            size += item->get_InmutableHeight() + separatorMargin;
        }

        return size - separatorMargin;
    }

    /**
     * Compute the width of all items. The result width will resize the dockwindow width.
     * referenced by DockWindow.
     * @return The width of the dockwindow.
     */
    guint get_dockItemsWidth()
    {
        guint separatorMargin = Configuration::get_separatorMargin();
        guint size = DockWindow::get_dockWindowStartEndMargin();

        for (auto item : AppUpdater::m_dockitems) {
            size += item->get_Width() + separatorMargin;
        }

        return size - separatorMargin;
    }

    /**
     * Compute the height of all items. The result width will resize the dockwindow height.
     * referenced by DockWindow.
     * @return the height of the dockwindow.
     */
    guint get_dockItemsHeight()
    {
        guint separatorMargin = Configuration::get_separatorMargin();
        guint size = DockWindow::get_dockWindowStartEndMargin();

        for (auto item : AppUpdater::m_dockitems) {
            if (item->m_dockitemtype == DockItemType::Separator){
                size += item->get_Width() + separatorMargin;
                continue;
            }

            size += item->get_Height() + separatorMargin;
        }

        return size - separatorMargin;
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
                    x -= (width / 2) - citem->get_Width()  / 2;
                    return true;
                }

                x += citem->get_Width() + Configuration::get_separatorMargin();
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

            int variantItemHeight = 0;
            for (DockItem* citem : AppUpdater::m_dockitems){

                variantItemHeight = citem->m_dockitemtype == DockItemType::Separator ? citem->get_Width() : citem->get_Height();

                if( citem->m_index == item->m_index){
                //    g_print("index %d/%d   center: %d \n", item->m_index, index, (height / 2) - citem->get_Height()  / 2);

                    y -= (height / 2) - variantItemHeight / 2;
                    return true;
                }

                y += variantItemHeight + Configuration::get_separatorMargin();
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

