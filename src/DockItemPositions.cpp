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
        int  requiredSize = get_inmutableItemsHeight() +  Configuration::get_CellHeight() ;
        int diff =  DockWindow::Monitor::get_workarea().height - requiredSize;

        if (diff < 0 ){
            return abs(diff) / (AppUpdater::m_dockitems.size());
        }

        return 0;
    }

    /**
     *  calculates the width size of all items. Returns the decrement value to resize the items.
     */
    guint get_ResizeWidthDecrement()
    {
        int  requiredSize = get_inmutableItemsWidth() +  Configuration::get_CellWidth();
        int diff =  DockWindow::Monitor::get_workarea().width  - requiredSize;

        if (diff < 0 ){
            return  abs(diff) / AppUpdater::m_dockitems.size();
        }

        return 0;
    }

    /**
     * Compute the original width of all items.
     * @return The width of all items.
     */
    guint get_inmutableItemsWidth()
    {
        guint separatorMargin = Configuration::get_separatorMargin();
        guint size = 0;

        for (auto item : AppUpdater::m_dockitems) {
            size += item->get_InmutableWidth() + separatorMargin;
        }

        return size - separatorMargin + Configuration::get_CellWidth() / 2;
    }

    /**
     * Compute the original height of all items.
     * @return The height of all items.
     */
    guint get_inmutableItemsHeight()
    {
        guint separatorMargin = Configuration::get_separatorMargin();
        guint size = 0 ;

        for (auto item : AppUpdater::m_dockitems) {
            if (item->m_dockitemtype == DockItemType::Separator) {
                size += item->get_InmutableWidth() + separatorMargin;
                continue;
            }

            size += (item->get_InmutableHeight() + separatorMargin);
        }

        return size - separatorMargin + Configuration::get_CellHeight() / 2;
    }

    /**
     * Compute the width of all items. The result width will resize the dockwindow width.
     * referenced by DockWindow.
     * @return The width of the dockwindow.
     */
    guint get_dockItemsWidth()
    {
        guint separatorMargin = Configuration::get_inmutableSeparatorMargin();
        guint size = DockWindow::get_dockWindowStartEndMargin();

        for (auto item : AppUpdater::m_dockitems) {
            size += item->get_width() + separatorMargin;
        }

        guint result =  size - separatorMargin;

        if (result > DockWindow::Monitor::get_workarea().width ) {
            result = DockWindow::Monitor::get_workarea().width;
        }

        return result;
    }

    /**
     * Compute the height of all items. The result width will resize the dockwindow height.
     * referenced by DockWindow.
     * @return the required height of the dockwindow.
     */
    guint get_dockItemsHeight()
    {
        guint separatorMargin = Configuration::get_inmutableSeparatorMargin();
        guint size = DockWindow::get_dockWindowStartEndMargin();

        for (auto item : AppUpdater::m_dockitems) {
            if (item->m_dockitemtype == DockItemType::Separator){
                size += item->get_width() + separatorMargin;
                continue;
            }

            size += item->get_height() + separatorMargin;
        }

        guint result =  size - separatorMargin;

        if (result > DockWindow::Monitor::get_workarea().height ) {
            result = DockWindow::Monitor::get_workarea().height; // - DockWindow::Monitor::get_workarea().y;
        }

        return result;
    }


    /*
     * returns the start position from the given item.
     * The position is the absolute position from the current monitor.
     * @param: DockItem, x , y and the  width of the source window.
     * @references: the start position. x and y. Depending of the DockWindow location.
     */
    bool get_CenterPosition(const gint index, gint& x, gint& y, const gint width, const gint height)
    {
        if (index < 0 || index > AppUpdater::m_dockitems.size()){
            return false;
        }

        DockItem* item = AppUpdater::m_dockitems[index];
        DockWindow::get_DockWindowPosition(x, y);

        DockWindow::Monitor::updateGeometry();

        if (DockWindow::is_Horizontal()){

            x += DockWindow::get_dockWindowStartEndMargin() / 2;
            if (Configuration::get_dockWindowLocation() == panel_locationType::BOTTOM){
                y -= height;
            }
            else{
                y += DockWindow::get_DockWindowHeight() ;
            }

            for (DockItem* citem : AppUpdater::m_dockitems) {
                if( citem->m_index == item->m_index){
                    x -= width / 2 - citem->get_width()  / 2;

                    // check the limit on the left
                    if (x <  DockWindow::Monitor::get_workarea().x) {
                        x =  DockWindow::Monitor::get_workarea().x;
                        return true;
                    }

                    // check the limit on the right
                    if (x + width  > DockWindow::Monitor::get_workarea().width) {
                        x =  DockWindow::Monitor::get_workarea().width - width + DockWindow::Monitor::get_workarea().x;
                        return true;
                    }

                    return false;
                }

                x += citem->get_width() + Configuration::get_separatorMargin();
            }
        }
        else
        {
            y += DockWindow::get_dockWindowStartEndMargin() / 2;
            if (Configuration::get_dockWindowLocation() == panel_locationType::RIGHT){
                x -= width;
            }
            else{
                x += DockWindow::get_DockWindowWidth();
            }

            int variantItemHeight = 0;
            for (DockItem* citem : AppUpdater::m_dockitems){

                variantItemHeight = citem->m_dockitemtype == DockItemType::Separator ? citem->get_width() : citem->get_height();

                if ( citem->m_index == item->m_index){

                    y -= height / 2 - variantItemHeight / 2;

                    g_print("Center %d/%d\n", y, x);

                    // check the limit on the top
                    if (y <  DockWindow::Monitor::get_workarea().y){
                        y =  DockWindow::Monitor::get_workarea().y;

                        return true;
                    }

                    // check the limit on the bottom
                    if (y + height  > DockWindow::Monitor::get_workarea().height){
                        y =  DockWindow::Monitor::get_workarea().height - height + DockWindow::Monitor::get_workarea().y;

                        return true;
                    }

                    return false;
                }

                y += variantItemHeight + Configuration::get_separatorMargin();
            }
        }

        return true;
    }

    /**
     * Returns the center of the current monitor in x, y coordinates.
     */
    void get_CenterScreenPos(int targetwidth, int targetheight, int &posx, int &posy)
    {
        int monitorWidth = DockWindow::Monitor::get_workarea().width;
        int monitorHeight = DockWindow::Monitor::get_workarea().height;

        int monitorcenterWidth = monitorWidth / 2;
        int monitorcenterHeight = monitorHeight / 2;


        posx = monitorcenterWidth - (targetwidth / 2);
        posy = monitorcenterHeight - (targetheight / 2);

    }

    int getCenter(int count, int index, int width)
    {
        int monitorWidth = DockWindow::Monitor::get_workarea().width;
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

