/*
 * File:   AppWindow.cpp
 * Author: yoo
 *
 * Created on November 3, 2018, 2:45 PM
 */

#include <cairo/cairo.h>


//#include <libwnck-3.0/libwnck/window.h>

//#include <libwnck-3.0/libwnck/window.h>

//#include <libwnck-3.0/libwnck/window.h>

#include "AppWindow.h"
#include "../config.h"
#include "DockWindowHandler.h"
#include "WindowControl.h"
#include "Animations.h"
#include "Utilities.h"

AppWindow::AppWindow() {
    add_events(Gdk::BUTTON_PRESS_MASK);
    set_app_paintable(true);

    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    //#region test

    this->set_title(PACKAGE_NAME); //#endregion

    // The App Logo
    //std::string iconFile = Utilities::getExecPath(DEF_LOGONAME);
    //this->set_icon_from_file(iconFile);

    // A window to implement a docking bar.
    // Used for creating the dock panel.
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
    /*
     Members
    GDK_WINDOW_TYPE_HINT_NORMAL

 	 
    GDK_WINDOW_TYPE_HINT_DIALOG

 	 
    GDK_WINDOW_TYPE_HINT_MENU

 	 
    GDK_WINDOW_TYPE_HINT_TOOLBAR

 	 
    GDK_WINDOW_TYPE_HINT_SPLASHSCREEN

 	 
    GDK_WINDOW_TYPE_HINT_UTILITY

 	 
    GDK_WINDOW_TYPE_HINT_DOCK

 	 
    GDK_WINDOW_TYPE_HINT_DESKTOP

 	 
    GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU

 	 
    GDK_WINDOW_TYPE_HINT_POPUP_MENU

 	 
    GDK_WINDOW_TYPE_HINT_TOOLTIP

 	 
    GDK_WINDOW_TYPE_HINT_NOTIFICATION

 	 
    GDK_WINDOW_TYPE_HINT_COMBO

 	 
    GDK_WINDOW_TYPE_HINT_DND
 
 
     */
    //_NET_WM_TYPE_DOCK
    this->set_decorated(false);

    // Add the dock panel
    this->add(m_dockpanel);



    // it must be shown before changing properties.
    // also need to call show to get a valid gdk_window

    //  this->show();
    // this->show_all();
    //  this->add_events(Gdk::PROPERTY_CHANGE_MASK);
    // Set masks for mouse events
    this->add_events(
            Gdk::PROPERTY_CHANGE_MASK |
            Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::BUTTON_PRESS_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
            Gdk::FOCUS_CHANGE_MASK |
            Gdk::ENTER_NOTIFY_MASK |
            Gdk::LEAVE_NOTIFY_MASK |
            Gdk::POINTER_MOTION_MASK);

    /*
    this->set_events(GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK |
GDK_POINTER_MOTION_HINT_MASK |
GDK_BUTTON_MOTION_MASK |
GDK_BUTTON1_MOTION_MASK |
GDK_BUTTON2_MOTION_MASK |
GDK_BUTTON3_MOTION_MASK |
GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
| GDK_KEY_PRESS_MASK |
GDK_KEY_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK |
GDK_FOCUS_CHANGE_MASK | GDK_STRUCTURE_MASK |
GDK_PROPERTY_CHANGE_MASK | GDK_VISIBILITY_NOTIFY_MASK |
GDK_PROXIMITY_IN_MASK | GDK_PROXIMITY_OUT_MASK | GDK_SUBSTRUCTURE_MASK |
GDK_SCROLL_MASK | GDK_TOUCH_MASK | GDK_SMOOTH_SCROLL_MASK );
     */
}

void AppWindow::LeaveFunc(GtkWidget *widget, GdkEvent *event, gpointer callback_data) {
    // if (event==(GdkEvent *)NULL || event->type!=GDK_LEAVE_NOTIFY) {
    // ErrorFunction( "LeaveFunc: NULL event or wrong type\n" );
    // return; /* bogus event */
    // }
    /* event is good */

}

int AppWindow::init() {

    //
    //    GdkScreen *screen = gdk_screen_get_default();
    WnckScreen *wnckscreen = wnck_screen_get(0);
    //
    //    // needed for "configure-event"
    //    this->add_events(Gdk::STRUCTURE_MASK);
    //
    //    // The monitors-changed signal is emitted when the number,
    //    // size or position of the monitors attached to the screen change.
    //    g_signal_connect(screen, "monitors-changed",
    //            G_CALLBACK(monitor_size_changed_callback),
    //            (gpointer) this);


    //g_signal_connect (G_OBJECT (this), "property_change",
    // G_CALLBACK(AppWindow::LeaveFunc), NULL);

    g_signal_connect(wnckscreen, "active_window_changed",
            G_CALLBACK(AppWindow::on_active_window_changed_callback), NULL);



    g_signal_connect(G_OBJECT(wnckscreen), "window_opened",
            G_CALLBACK(window_opened_callback), NULL);

    g_signal_connect(G_OBJECT(wnckscreen), "window_opened",
            G_CALLBACK(window_opened_callback), NULL);


    //   g_signal_connect(window, "geometry-changed",
    //                 G_CALLBACK(geometry_changed), NULL);
    // wnck_
    // Initialize the monitor geometry.
    //https://lazka.github.io/pgi-docs/Wnck-3.0/classes/Window.html

    this->show_all();

    if (DockWindow::init(this) != 0)
        return -1;

    DockWindow::show();

    if (Configuration::is_activateStrut()) {
        DockWindow::updateStrut();
    }


    //    // repositioning10 the window
    //    if (DockWindow::updateStrut() != 0)
    //        return -1;

    Glib::signal_timeout().connect(sigc::mem_fun(*this,
            &AppWindow::on_timeout), 100);

    Glib::signal_timeout().connect(sigc::mem_fun(*this,
            &AppWindow::autoHideTimer), DEF_FRAMERATE);

    initTime = 0; //DockWindow::get_geometry().height-100;;
    // endPosition = DockWindow::get_geometry().height ;//- get_height();//Configuration::get_dockWindowSize();//+180);//DockWindow::get_geometry().height; //this->
    //1080
    endPosition = Configuration::get_dockWindowSize(); //+180);//DockWindow::get_geometry().height; //this->
    g_print("%f\n", endPosition);

    //time=10;
    return 0;
}
//bool visible = true;
bool mouseIn = false;
bool animate = false;

bool AppWindow::autoHideTimer() {

    if (Configuration::is_autoHide() == false) {
        return true;
    }

    if (!animate && mouseIn && DockWindow::is_visible()) {
        m_Timer.start();
        return true;
    }

    int x, y;
    Utilities::getMousePosition(x, y);
    bool restore = false;

    if (!mouseIn && !DockWindow::is_visible()) {
        switch (Configuration::get_dockWindowLocation()) {
            case panel_locationType::TOP: restore = y < 6;
                break;
            case panel_locationType::BOTTOM: restore = y >= DockWindow::get_geometry().height - 6;
                break;
            case panel_locationType::LEFT: restore = x < 6;
                break;
            case panel_locationType::RIGHT: restore = x >= DockWindow::get_geometry().width - 6;
                break;
        };
    }

    if (!animate && !DockWindow::is_visible() && restore) {
        animate = true;

        m_easing_duration = 1.f;
        
       //  DockWindow::updateStrut(Configuration::get_dockWindowSize());
    }

    if (!animate && DockWindow::is_visible() && !mouseIn && m_Timer.elapsed() > 2.5) {
        animate = true;
         m_easing_duration = 16.f;
        if (Configuration::is_activateStrut()) {
            DockWindow::updateStrut(1);
        }
    }

    if (animate) {
        m_Timer.stop();
      //  auto duration = 16.f;//m_easing_duration;
        auto endTime = initTime + m_easing_duration;
        auto now = atime;
        int currentPositionX = 0;
        int currentPositionY = 0;
        int startPosition = 0;

        position = ofxeasing::map_clamp(now, initTime, endTime, 0, endPosition, &ofxeasing::linear::easeIn);

        switch (Configuration::get_dockWindowLocation()) {
            case panel_locationType::TOP:
                if (DockWindow::is_visible()) {
                    startPosition = 0; //Configuration::get_dockWindowSize();
                    currentPositionY = startPosition - position;

                } else {
                    startPosition = -Configuration::get_dockWindowSize();
                    currentPositionY = startPosition + position;
                }

                break;
            case panel_locationType::BOTTOM:
                if (DockWindow::is_visible()) {
                    startPosition = (DockWindow::get_geometry().height - Configuration::get_dockWindowSize());
                    currentPositionY = startPosition + position;

                } else {
                    startPosition = (DockWindow::get_geometry().height);
                    currentPositionY = startPosition - position;
                }
                break;
            case panel_locationType::LEFT:
                if (DockWindow::is_visible()) {
                    startPosition = 0;
                    currentPositionX = startPosition - position;

                } else {
                    startPosition = -Configuration::get_dockWindowSize();
                    currentPositionX = startPosition + position;
                }

                break;

            case panel_locationType::RIGHT:
                if (DockWindow::is_visible()) {
                    startPosition = (DockWindow::get_geometry().width - Configuration::get_dockWindowSize());
                    currentPositionX = startPosition + position;

                } else {
                    startPosition = (DockWindow::get_geometry().width);
                    currentPositionX = startPosition - position;
                }
                break;

        };


        move(currentPositionX, currentPositionY);
        //  g_print("%f - %f > %f\n", currentPosition, position, endPosition);
       // g_print("%d\n", currentPositionX);


        //  g_print("%f - %f > %f\n", currentPosition, position, endPosition);


        if (atime < m_easing_duration) {
            atime++;
        }


        if (abs(position) >= abs(endPosition))
            //   if (currentPosition >=  endPosition)
        {
            switch (Configuration::get_dockWindowLocation()) {
                case panel_locationType::TOP:
                    DockWindow::set_visible(abs(currentPositionY) == 0); // > Configuration::get_dockWindowSize());
                    break;
                case panel_locationType::BOTTOM:
                    DockWindow::set_visible(currentPositionY < DockWindow::get_geometry().height);

                    break;
                case panel_locationType::LEFT:
                    DockWindow::set_visible(abs(currentPositionX) == 0); // > Configuration::get_dockWindowSize());
                    break;
                case panel_locationType::RIGHT:
                    DockWindow::set_visible(currentPositionX != DockWindow::get_geometry().width);

            };


            if (Configuration::is_activateStrut()) {
                if (DockWindow::is_visible()) {
                    DockWindow::updateStrut();
                }
            }


            initTime = 0;
            atime = 0;
            animate = false;
          //  g_print("currpos: %d %d\n", currentPositionX, currentPositionY);
            // exit(1);
            //  if( DockWindow::is_visible())
            // 

        }
    }

    return true;
}

bool isfullscreenSet = false;
bool isfullscreen = false;

bool AppWindow::on_timeout() {
    return true;

    ////    if(isfullscreenSet && isfullscreen)
    //   return true;

    //     int x=0;
    //        int y=0;
    //
    //        this->get_position(x,y);
    //        if(isfullscreen && y < MonitorGeometry::GetGeometry().height){
    //         return true;
    //        }
    isfullscreen = WindowControl::FullscreenActive();


    if (isfullscreen && !isfullscreenSet) {

        g_print("isfullscreen\n");
        DockWindow::hide();

        isfullscreenSet = true;

        return true;

    }

    if (!isfullscreen && isfullscreenSet) {
        //  isfullscreenSet=false;
        isfullscreenSet = false;
        //        if(!isfullscreenSet)
        ////            return true;
        //        int x=0;
        //        int y=0;
        //        this->get_position(x,y);

        //        if(y != DockWindow::GetGeometry().height)
        //        {
        //            return true;
        //        }

        g_print("normal\n");

        DockWindow::show();
        isfullscreenSet = false;
    }

    return true;
}

void AppWindow::monitor_size_changed_callback(GdkScreen *screen, gpointer gtkwindow) {
    g_info("Monitor settings changed\n");
    // Adapt the monitor geometry and repositioning the window
    //  MonitorGeometry::update((Gtk::Window*)gtkwindow);

}

void AppWindow::on_active_window_changed_callback(WnckScreen *screen,
        WnckWindow *previously_active_window, gpointer user_data) {
    // m_currentMoveIndex = -1;

    // if (m_previewWindowActive || DockPanel::m_dragdropsStarts)
    //     return;

    WnckWindow *window = wnck_screen_get_active_window(screen);


    // g_print("\n");
    // g_print(wnck_window_get_name(window) );

}

void AppWindow::geometry_changed(WnckWindow *window) {
    // if (wnck_window_is_maximized(window)) {
    g_print("A window has been maximized\n");
    // }
}

void AppWindow::window_opened_callback(WnckScreen* screen, WnckWindow* window, gpointer data) {
    //g_print("open\n");
    // g_print(wnck_window_get_name(window) );

    //  g_signal_connect(window, "geometry-changed",
    //         G_CALLBACK(AppWindow::window_geometry_changed_callback), NULL);

    // if (wnck_window_is_maximized(window)) {
    //
    //    }
}

WnckWindow *fullscreen_window = NULL;

void AppWindow::window_geometry_changed_callback(WnckWindow *window, gpointer user_data) {
    // if( wnck_window_is_fullscreen(window))
    g_print("geo change\n");




    isfullscreen = WindowControl::FullscreenActive();

    if (isfullscreen) {
        fullscreen_window = window;
        DockWindow::hide();
        return;
    }

    if (isfullscreen && window == fullscreen_window) {
        fullscreen_window = NULL;
        DockWindow::show();
    }

    return;

    if (isfullscreen && !isfullscreenSet) {

        g_print("isfullscreen\n");
        DockWindow::hide();

        isfullscreenSet = true;

        return;

    }

    if (!isfullscreen && isfullscreenSet) {
        //  isfullscreenSet=false;
        isfullscreenSet = false;
        //        if(!isfullscreenSet)
        ////            return true;
        //        int x=0;
        //        int y=0;
        //        this->get_position(x,y);

        //        if(y != DockWindow::GetGeometry().height)
        //        {
        //            return true;
        //        }

        //    g_print("normal\n");

        DockWindow::show();
        isfullscreenSet = false;
    }

}

bool AppWindow::on_enter_notify_event(GdkEventCrossing* crossing_event) {
    g_print("IN\n");
    mouseIn = true;
    return true;
}

bool AppWindow::on_leave_notify_event(GdkEventCrossing* crossing_event) {
    mouseIn = false;
    g_print("OUT\n");
    return true;
}

/*
bool AppWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {


    //Gtk::Allocation allocation = get_allocation();
    //const int width = allocation.get_width();
    //const int height = allocation.get_height();

    //Glib::RefPtr<Gdk::Pixbuf> icon = NULLPB;

    cr->set_source_rgba(0.0, 0.0, 0.8, 0.6); // partially translucent
    //  cr->rectangle(0, 0, double(width)/10, double(height)/10);
    cr->paint();

    return true;
}
 */


AppWindow::~AppWindow() {
}
