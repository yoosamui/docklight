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
#include "AppRunAnimation.h"
#include "TitleWindow.h"





class DockPanel : public Gtk::DrawingArea {
public:
    DockPanel();
    virtual ~DockPanel();
//    float m_easing_duration;
    SessionWindow* m_sessionWindow;
   // guint get_dockItemsWidth();
   // guint get_dockItemsHeight();

    int get_CurrentIndex() {
        return m_currentMoveIndex;
    };
    int Init();
    void update();
private:

    static bool m_forceDraw;    

    TitleWindow m_titlewindow;
    TitleWindow m_infowindow;
    
    AppUpdater m_appUpdater;
    std::string m_homeiconFilePath;
    static int m_currentMoveIndex;
    int get_Index(const int& mouseX, const int& mouseY);
    int m_previousCellwidth;
    int m_iconsize;
    bool m_mouseLeftButtonDown = false;
    bool m_mouseRightButtonDown = false;
    float m_mouseclickEventTime = 0.0f;
   
    
    DockItem* get_CurrentItem();
    
    // Timer for showing the title window
    Glib::Timer m_titleTimer;
    
    gdouble m_titleElapsedSeconds;
    int m_titleItemOldindex = 0;
    bool m_titleShow = false;
    void get_ItemPosition(const DockItemType dockType, int& x, int& y, int& width, int& height);

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
    //Override default signal handler:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    void RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
            double x, double y, double width, double height, double radius);

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

    void draw_Panel(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_Items(const Cairo::RefPtr<Cairo::Context>& cr);
    void show_Title();


    // Animation thread related
    std::thread* m_AppRunThreadLauncher = nullptr;
    static void AppRunAnimation();
    static Glib::RefPtr<Gdk::Pixbuf> m_AppRunImage;
    static bool m_AppThreadRunning;

    // Mome menu
    Gtk::Menu m_HomeMenu;

    Gtk::MenuItem m_QuitMenuItem;
    void on_QuitMenu_event();

  // void on_menuNew_event();
 //callback slot
    void on_popup_homemenu_position(int& x, int& y, bool& push_in);



     // Item menu
    Gtk::Menu m_ItemMenu;
    Gtk::MenuItem m_MenuItemNewApp;
    Gtk::MenuItem m_MenuItemAttach;
    Gtk::MenuItem m_MenuItemDetach;
    void on_popup_itemmenu_position(int& x, int& y, bool& push_in);


    Gtk::SeparatorMenuItem m_separatorMenuItem0;

};

#endif	/* DOCKPANEL_H */

