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


#ifndef DOCKITEM_H
#define	DOCKITEM_H


#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>

#include <gtkmm/window.h>
#include <glibmm/timer.h>
#include "Configuration.h"

typedef enum DockItem_t {
    SingleDock = 0,
    GroupDock = 1,
    Separator = 2

} DockItemType;

class DockItem {
public:
    DockItem();
    DockItem(const guint, const guint height);
    virtual ~DockItem();
    DockItemType m_dockitemtype = DockItemType::SingleDock;
    Glib::RefPtr<Gdk::Pixbuf> m_image = NULLPB;
    GdkPixbuf *m_scaledPixbuf = nullptr;
    guint m_scaleWidth = 0;
    guint m_scaleHeight = 0;
    bool m_isDynamic = false;
    DockItem* get_Next();
    WnckWindow *m_window;
    std::string m_theme_iconname;
    std::string m_titlename;
    std::string m_appname;
    std::string m_realgroupname;
    std::string m_instancename;
    std::vector<DockItem*> m_items;
    int m_xid;
    int m_index = 0;
    gint m_posX = 0;
    gint m_posY = 0;
    bool m_isAttached = false;
    bool m_isDirty = false;
    bool visible = true;
    int m_frames;
    std::string get_Title();
    std::string get_GroupName();
    std::string get_DesktopFileName();
    int m_dockitemSesssionGrpId = 0;
    int get_Width();
    int get_Height();
    gint get_InmutableWidth();
    gint get_InmutableHeight();

private:
    bool m_pixbufPreviousPass;

    gint m_width = 0;
    gint m_height = 0;

};

#endif	/* DOCKITEM_H */

