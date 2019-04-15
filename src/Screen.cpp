/* 
 * File:   Screen.cpp
 * Author: yoo
 * 
 * Created on January 5, 2019, 2:35 PM
 */

#include "Screen.h"

/**
 * This class manage single and dual monitors geometry.
 * @param window
 */
Screen::Screen()
{
    
}

void Screen::init(Gtk::Window* window)
{
    if (window == NULL) {
        g_critical("Argument is null.");
        return;
    }
    this->toplevel = gtk_widget_get_toplevel(GTK_WIDGET(window->gobj()));
    this->gdk_window = gtk_widget_get_window(this->toplevel);
    if (this->gdk_window == NULL) {
        g_critical("Screen gdk_window is null.");
        return;
    }

   
    this->window = window;
    this->update();
    
    this->struts = new Struts(window, this);
}

Screen::~Screen(){ }

std::vector<MonitorObject*> Screen::get_Monitors()
{
    return this->monitors;
}

void Screen::update()
{

    this->monitors.clear();
    GdkDisplay* m_defaultdisplay = gdk_display_get_default();
    auto count = gdk_display_get_n_monitors(m_defaultdisplay);
    for (int i = 0; i < count; i++) {
        MonitorObject* m = new MonitorObject();
        m->index = i;
        m->monitor = gdk_display_get_monitor(m_defaultdisplay, i);
        m->primary = gdk_monitor_is_primary(m->monitor) ? true : false;

        
        
        // Retrieves the size and position of the “work area” on a monitor within the display coordinate space.
        // The returned geometry is in ”application pixels”, not in ”device pixels” (see gdk_monitor_get_scale_factor()).
        // The work area should be considered when positioning menus and similar popups, to avoid placing them below panels,
        // docks or other desktop components.
        // Note that not all backends may have a concept of workarea. 
        // This function will return the monitor geometry if a workarea is not available, or does not apply.
        gdk_monitor_get_workarea(m->monitor, &m->workarea);

        // GdkRectangle to be filled with the monitor geometry
        gdk_monitor_get_geometry(m->monitor, &m->geometry);

        if (m->primary) {
            this->primaryMonitor = m;
        }
        else {
            this->nextMonitor = m;
        }

        this->monitors.push_back(m);
    }
}

MonitorObject* Screen::get_PrimaryMonitor()
{

  //  if(this->window != nullptr){
 // auto yy =  this->monitors[0]->geometry.height;
//g_print(".....................GEOME %d\n",yy );
//  }

    return this->primaryMonitor;
}

MonitorObject* Screen::get_NextMonitor()
{
    return this->nextMonitor;
}

gint Screen::get_MonitorsCount()
{
    return  this->monitors.size();
}
