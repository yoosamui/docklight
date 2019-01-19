/* 
 * File:   Screen.h
 * Author: yoo
 *
 * Created on January 5, 2019, 2:35 PM
 */

#ifndef SCREEN_H
#define	SCREEN_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <gtkmm.h>
#include <gdk/gdk.h> 
#include "vector"
#include "Struts.h"

class MonitorObject {
public:
    GdkMonitor* monitor;
    GdkRectangle workarea;
    GdkRectangle geometry;
    int index;
    bool primary = false;
};


class Screen {
public:
    Screen();
    void init(Gtk::Window* window);
    virtual ~Screen();
    void update();
    MonitorObject* get_PrimaryMonitor();
    MonitorObject* get_NextMonitor();
    gint get_MonitorsCount();
    Struts* struts;
private:
    MonitorObject* primaryMonitor;
    MonitorObject* nextMonitor;
    Gtk::Window* window;
    std::vector<MonitorObject*> monitors;
    //GdkDisplay* defaultdisplay;
    GdkWindow* gdk_window;
    GtkWidget* toplevel;
   
    std::vector<MonitorObject*> get_Monitors();

};

#endif	/* SCREEN_H */

