/* 
 * File:   DockItem.h
 * Author: yoo
 *
 * Created on November 11, 2018, 5:14 PM
 */

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
    DockItem(const DockItem& orig);
    virtual ~DockItem();
    DockItemType m_dockitemtype = DockItemType::SingleDock;
   
    Glib::RefPtr<Gdk::Pixbuf> m_image;
    guchar m_pixelsbuf[DEF_PREVIEW_WIDTH * DEF_PREVIEW_HEIGHT * 3] = {0};
    GdkPixbuf *m_scaledPixbuf;
    bool m_imageLoadedRequired;
    bool m_isDynamic;
    DockItem* GetCurrent();
    DockItem* GetNext();
    WnckWindow *m_window;
    std::string m_theme_iconname;
    std::string m_titlename;
    std::string m_appname;
    std::string m_realgroupname;
    std::string m_instancename;
    std::vector<DockItem*> m_items;
    int m_xid;
    int m_index = 0;
    int m_attachedIndex = 0;
    bool m_isAttached = false;
    bool m_isDirty = false;
    bool visible = true;
    int m_frames;
    std::string getTitle();
    std::string getGroupName();
    std::string getDesktopFileName();
    gboolean isMovementDetected(GdkPixbuf *pixbuf);
    int m_dockitemSesssionGrpId = 0;
private:
    bool m_pixbufPreviousPass;
    

};

#endif	/* DOCKITEM_H */

