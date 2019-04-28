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
    guint get_inmutableItemsWidth();
    guint get_inmutableItemsHeight();
    guint get_ResizeHeightDecrement();
    guint get_ResizeWidthDecrement();
    bool get_CenterPosition(const int index, int& x, int& y, const guint width, const guint height);
    void get_CenterScreenPos(int targetwidth, int targetheight, int &posx, int &posy);
}

#endif	/* DOCKITEMPOSITIONS_H */

