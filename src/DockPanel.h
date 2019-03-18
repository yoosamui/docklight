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
#include "AppUpdater.h"

class DockPanel : public Gtk::DrawingArea {
public:
    DockPanel();


    float m_easing_duration;
    virtual ~DockPanel();
    SessionWindow* m_sessionWindow;


    gint get_dockItemsWidth();
    gint get_dockItemsHeight();

    int getCurrentIndex() {
        return m_currentMoveIndex;
    };

    int preInit(Gtk::Window* window);
   
private:

    //static Gtk::Window* m_AppWindow;
    AppUpdater m_appUpdater;

    std::string m_homeiconFilePath;
    static int m_currentMoveIndex;
    int getIndex(int x, int y);

    // int m_cellheight;
    // int m_cellwidth;
    int m_previousCellwidth;
    int m_iconsize;

    bool m_mouseLeftButtonDown = false;
    bool m_mouseRightButtonDown = false;
    float m_mouseclickEventTime = 0.0f;
    bool canShow();
   

protected:

    void ExecuteApp(GdkEventButton* event);
    void on_menuNew_event();
    
    static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);
    
    // Mouse handlers
    // http://www.horde3d.org/wiki/index.php5?title=Tutorial_-_Setup_Horde_with_Gtkmm  
    virtual bool on_button_press_event(GdkEventButton *event);
    virtual bool on_button_release_event(GdkEventButton *event);
    virtual bool on_motion_notify_event(GdkEventMotion* event);
    virtual bool on_scroll_event(GdkEventScroll* e);

    
    bool on_timeoutDraw();
    // Fix for transparency in Appwindow
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    //Override default signal handler:
//  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;


    void RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
            double x, double y, double width, double height, double radius);



    //void get_dockItemPosition(int &x1, int &y1, int &x2, int &y2, int &center, int increment);
    void get_dockItemPosition(DockItem* item, int &x1, int &y1, int &x2, int &y2, int &center, int i);

    virtual bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    virtual bool on_leave_notify_event(GdkEventCrossing* crossing_event);



    unsigned int get_dockItemsWidthUntilIndex(int idx);
    unsigned int get_dockItemsHeightUntilIndex(int idx);

    float m_time = 0;

    float position = 0;
    float initTime;
    float endPosition;


    bool m_mouseIn = false;
    bool m_animate = false;
    bool m_timerStoped = true;
    bool m_visible = true;
    float atime = 0.f;

    Glib::Timer m_Timer;


    void draw_Panel(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y);
    void draw_Items(const Cairo::RefPtr<Cairo::Context>& cr, int currentposX, int currentposY);
};

#endif	/* DOCKPANEL_H */

