/* 
 * File:   DockItemPositions.h
 * Author: yoo
 *
 * Created on March 24, 2019, 12:59 PM
 */

#ifndef DOCKITEMPOSITIONS_H
#define	DOCKITEMPOSITIONS_H

#include "DockItem.h"

namespace DockItemPositions
{
    guint get_dockItemsWidth();
    guint get_dockItemsHeight();
    void get_CenterPositionByItem(const DockItem& item, int& x, int& y, const guint width, const guint height);
    
    

    int getCenter(int count, int index, int width);

}


#endif	/* DOCKITEMPOSITIONS_H */

