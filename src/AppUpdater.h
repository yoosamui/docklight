/* 
 * File:   AppUpdater.h
 * Author: yoo
 *
 * Created on February 16, 2019, 12:52 PM
 */

#ifndef APPUPDATER_H
#define	APPUPDATER_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm.h> 

#include "Configuration.h"
#include "DockItem.h"
#include "SessionWindow.h"

class AppUpdater {
public:

    AppUpdater();
    virtual ~AppUpdater();

    static std::vector<DockItem*> m_dockitems;

protected:
    static void Update(WnckWindow* window, Window_action actiontype);
    static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);

private:

};

#endif	/* APPUPDATER_H */

