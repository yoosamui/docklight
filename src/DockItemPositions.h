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
    int getStartPos(const DockItem& item, const int count, const int index, const int width);
    int getCenter(int count, int index, int width);

}


#endif	/* DOCKITEMPOSITIONS_H */

