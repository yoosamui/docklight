/*
 * File:   WckHandler.h
 * Author: yoo
 *
 * Created on March 18, 2019, 6:01 PM
 */

#ifndef WNCKHANDLER_H
#define	WNCKHANDLER_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm/window.h>
#include "DockItem.h"

#include <X11/X.h>

namespace WnckHandler
{
    void ActivateWindow(WnckWindow* window);
    WnckWindow* get_ActiveWindowIfAny(DockItem* item);
    void SelectWindow(WnckWindow* window);
    void HomeCloseAllWindows();
    void HomeCloseAllExceptActive();
    void HomeMinimizeAllExceptActive();
    void HomeMinimizeAll();
    void HomeUnMinimizeAll();
    bool isExistsUnMaximizedWindowsByDockItem(DockItem* dockitem);
    bool isExistsMinimizedWindowsByDockItem(DockItem* dockitem);
    void closeAllExceptActiveByDockItem(DockItem* dockitem);
    void closeAllByDockItem(DockItem* dockitem);
    void minimizeAllExceptActiveByDockItem(DockItem* dockitem);
    void minimizeAllByDockItem(DockItem* dockitem);
    void unMinimizeAllByDockItem(DockItem* dockitem);
    bool isExitsActivetWindowByDockItem(DockItem* dockitem);
    int isExitstWindowsByDockItem(DockItem* dockitem);
    int windowscount();
//   int getWindowsByName(const std::string& appname, std::vector<windowData>& data);
//    WnckWindow* getWindowByName(const std::string& appname, std::string& tittle);
    WnckWindow* get_Active();
    bool isWindowExists(XID xid);
    bool isExistsMinimizedWindows();
    void hideWindow(Gtk::Window* instance);
    int minimizedWindowscount();
    int unMinimizedWindowsCount();
    bool FullscreenActive();
    WnckWindow* get_ExistingWindowDock();
    std::string get_windowName(WnckWindow* window);
}

#endif	/* WNCKHANDLER_H */

