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
    bool get_previewSize(const guint itemsSize, guint& windowWidth, guint& windowHeight, guint& cellWidth, guint& cellHeight);

    bool get_CenterPosition(const gint index, gint& x, gint& y, const gint width, const gint height);
    void get_CenterScreenPos(int targetwidth, int targetheight, int &posx, int &posy);
}

#endif	/* DOCKITEMPOSITIONS_H */

