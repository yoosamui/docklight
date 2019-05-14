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
    guint m_posX = 0;
    guint m_posY = 0;
    bool m_isAttached = false;
    bool m_isDirty = false;
    bool visible = true;
    int m_frames;
    std::string get_Title();
    std::string get_GroupName();
    std::string get_DesktopFileName();
    int m_dockitemSesssionGrpId = 0;
    guint get_Width();
    guint get_Height();
    guint get_InmutableWidth();
    guint get_InmutableHeight();

private:
    bool m_pixbufPreviousPass;

    guint m_width = 0;
    guint m_height = 0;

};

#endif	/* DOCKITEM_H */

