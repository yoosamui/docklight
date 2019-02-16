/* 
 * File:   AppUpdater.cpp
 * Author: yoo
 * 
 * Created on February 16, 2019, 12:52 PM
 */

#include "AppUpdater.h"
#include "DockWindow.h"
#include "Launcher.h"

// static members
std::vector<DockItem*> AppUpdater::m_dockitems;

AppUpdater::AppUpdater()
{

    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();
    
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                     G_CALLBACK(AppUpdater::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
                     G_CALLBACK(AppUpdater::on_window_closed), NULL);

}

AppUpdater::~AppUpdater(){ }

/**
 * Emitted when a new Wnck.Window is opened on screen.
 * @param screen
 * @param window
 * @param data
 */
void AppUpdater::on_window_opened(WnckScreen *screen, WnckWindow* window, gpointer data)
{
    Update(window, Window_action::OPEN);
}

/**
 * Emitted when a Wnck.Window is closed on screen.
 * @param screen
 * @param window
 * @param data
 */
void AppUpdater::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    // updateSessioWindow(window);
    Update(window, Window_action::CLOSE);
}

void AppUpdater::Update(WnckWindow* window, Window_action actiontype)
{
    WnckWindowType wt = wnck_window_get_window_type(window);

    if (wt == WNCK_WINDOW_DESKTOP ||
        wt == WNCK_WINDOW_DOCK ||
        wt == WNCK_WINDOW_MENU ||
        wt == WNCK_WINDOW_SPLASHSCREEN) {
        return;
    }

    if (actiontype == Window_action::OPEN) {
        std::string the_appname;
        std::string the_instancename;
        std::string the_groupname;
        std::string the_titlename;

        if (Launcher::getAppNameByWindow(window,
                                         the_appname,
                                         the_instancename,
                                         the_groupname,
                                         the_titlename) == FALSE) {
            return;
        }

        //DEBUG
        g_print("...appnames: %s, %s, %s title:%s\n",
                the_appname.c_str(),
                the_instancename.c_str(),
                the_groupname.c_str(),
                the_titlename.c_str());

        /*
   
                if (m_currentsessionItem != nullptr && m_sessiondata.size() > 0) {
                    for (int i = m_sessiondata.size() - 1; i >= 0; i--) {
                        std::string appname(m_sessiondata[i].appname);
                        if (appname == the_groupname) {
                            if (attachToSessiongrp(window)) {
                                m_sessiondata.erase(m_sessiondata.begin() + i);
                                break;
                            }
                        }
                    }
                }
         */


        // handle DockItems groups
        for (auto item:AppUpdater::m_dockitems) {
            if (the_groupname == item->m_realgroupname) {
                DockItem* childItem = new DockItem();
                childItem->m_realgroupname = the_groupname;
                childItem->m_appname = the_appname;
                childItem->m_titlename = the_titlename;
                childItem->m_instancename = the_instancename;
                childItem->m_isDirty = false;
                childItem->m_window = window;
                childItem->m_xid = wnck_window_get_xid(window);
                childItem->m_image = NULLPB;

                if (item->m_isAttached && item->m_isDirty) {
                    item->m_window = window;
                    item->m_xid = childItem->m_xid;
                    item->m_isDirty = false;

                    for (auto itemtorelease:item->m_items)
                        delete(itemtorelease);

                    item->m_items.clear();
                }

                item->m_items.push_back(childItem);
                return;
            }
        }


        Glib::RefPtr<Gdk::Pixbuf> appIcon = NULLPB;

        std::string theme_iconname = "";
        appIcon = IconLoader::GetWindowIcon(window, theme_iconname);
        appIcon = appIcon->scale_simple(DEF_ICONMAXSIZE,
                                        DEF_ICONMAXSIZE, Gdk::INTERP_BILINEAR);

        // Create a new Item
        DockItem* dockItem = new DockItem();
        dockItem->m_appname = the_appname;
        dockItem->m_titlename = the_titlename;
        dockItem->m_realgroupname = the_groupname;
        dockItem->m_instancename = the_instancename;
        dockItem->m_window = window;
        dockItem->m_xid = wnck_window_get_xid(window);
        dockItem->m_image = appIcon;
        dockItem->m_theme_iconname = theme_iconname;

        // Create a child items
        DockItem* childItem = new DockItem();
        childItem->m_appname = dockItem->m_appname;
        childItem->m_titlename = the_titlename;
        childItem->m_realgroupname = dockItem->m_realgroupname;
        childItem->m_instancename = dockItem->m_instancename;
        childItem->m_window = dockItem->m_window;
        childItem->m_xid = dockItem->m_xid;
        childItem->m_image = NULLPB;

        dockItem->m_items.push_back(childItem);
        m_dockitems.push_back(std::move(dockItem));

        //        if (m_AppWindow != nullptr) {
        //            ((AppWindow*)m_AppWindow)->update(true);
        //        }
        DockWindow::update();

    }
    else {
        // find the item to remove;
        int xid = wnck_window_get_xid(window);
        int i = 1;
        bool found = false;
        for (; i < (int)m_dockitems.size(); i++) {
            for (auto ci:m_dockitems.at(i)->m_items) {
                if (ci->m_xid == xid) {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (xid > 0 && found) {
            auto item = m_dockitems.at(i);
            if (item->m_items.size() == 1) {
                // if is attached delete the child item.
                if (item->m_isAttached) {
                    item->m_isDirty = true;

                    delete( item->m_items.at(0));
                    item->m_items.erase(item->m_items.begin());

                    DockWindow::update();
                    return;
                }

                // remove this item
                delete( item->m_items.at(0));
                m_dockitems.erase(m_dockitems.begin() + i);

                // if is not attached then it is at the end on the list.
                // we need to reset the index.
                /////////////////// m_currentMoveIndex = -1;

                DockWindow::update();
                return;
            }
            else {
                // search for the xid and remove the item
                int idx = 0;
                for (auto c:item->m_items) {
                    if (c->m_xid == xid) {
                        delete(c);
                        item->m_items.erase(item->m_items.begin() + idx);
                        DockWindow::update();
                        return;
                    }
                    idx++;
                }
            }
        }
    }


}

