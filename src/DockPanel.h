/* 
 * File:   DockPanel.h
 * Author: yoo
 *
 * Created on November 6, 2018, 7:31 PM
 */

#ifndef DOCKPANEL_H
#define	DOCKPANEL_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm.h> 

#include "Configuration.h"
#include "DockItem.h"
#include "SessionWindow.h"
class DockPanel : public Gtk::DrawingArea {
public:
    DockPanel();

    virtual ~DockPanel();
     SessionWindow* m_sessionWindow;
private:
   
panel_locationType m_location;
static std::vector<DockItem*> m_dockitems;
 static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
 
protected:

    
    bool on_timeoutEasing();
    bool on_timeoutDraw();
     // Fix for transparency in Appwindow
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

 void RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
            double x, double y, double width, double height, double radius);
 
 
 
 void get_dockItemPosition(int &x1, int &y1, int &x2 ,int &y2, int &center, int increment);
 
 virtual bool on_enter_notify_event(GdkEventCrossing* crossing_event);
 virtual bool on_leave_notify_event(GdkEventCrossing* crossing_event);
 
 
 float m_time          = 0;
 
 float position=0;
 float initTime;
		float endPosition;
};

#endif	/* DOCKPANEL_H */

