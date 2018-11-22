//*****************************************************************
//
//  Copyright (C) 2018 Juan R. Gonzalez
//  Created on November 3, 2018, 2:45 PM
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//*****************************************************************
#include <cairo/cairo.h>
#include "AppWindow.h"
#include "../config.h"
#include "DockWindowHandler.h"
#include "WindowControl.h"
#include "Animations.h"
#include "Utilities.h"

bool AppWindow::m_isfullscreenSet;
bool AppWindow::m_isfullscreen;

AppWindow::AppWindow()
{
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

}

void AppWindow::LeaveFunc(GtkWidget *widget, GdkEvent *event, gpointer callback_data)
{
    // if (event==(GdkEvent *)NULL || event->type!=GDK_LEAVE_NOTIFY) {
    // ErrorFunction( "LeaveFunc: NULL event or wrong type\n" );
    // return; /* bogus event */
    // }
    /* event is good */


    g_print("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    exit(1);

}

int AppWindow::init()
{


    this->set_title(PACKAGE_NAME);
    //std::string iconFile = Utilities::getExecPath(DEF_LOGONAME);
    //this->set_icon_from_file(iconFile);

    // A window to implement a docking bar.
    // Used for creating the dock panel.
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_decorated(false);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

    // Add the dock panel
    this->add(m_dockpanel);

    this->add_events(
                     Gdk::PROPERTY_CHANGE_MASK |
                     Gdk::POINTER_MOTION_HINT_MASK |
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


    //  g_signal_connect(G_OBJECT(wnckscreen), "property_change",
    //                 G_CALLBACK(AppWindow::LeaveFunc), NULL);

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
                                                 &AppWindow::fullScreenTimer), 100);

    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &AppWindow::autoHideTimer), DEF_FRAMERATE);

    initTime = 0; //DockWindow::get_geometry().height-100;;
    // endPosition = DockWindow::get_geometry().height ;//- get_height();//Configuration::get_dockWindowSize();//+180);//DockWindow::get_geometry().height; //this->
    //1080
    endPosition = Configuration::get_dockWindowSize(); //+180);//DockWindow::get_geometry().height; //this->
    // g_print("%f\n", endPosition);

    //time=10;
    return 0;
}
//bool visible = true;

bool AppWindow::autoHideTimer()
{

    if (Configuration::is_autoHide() == false /*|| m_isfullscreenSet == true*/) {
        return true;
    }

    if (!m_animate && m_mouseIn && DockWindow::is_visible()) {
        m_Timer.start();
        return true;
    }

    bool restore = false;
    int x, y;
    Utilities::getMousePosition(x, y);
    if (!m_mouseIn && !DockWindow::is_visible()) {
        switch (Configuration::get_dockWindowLocation())
        {
            case panel_locationType::TOP:
                restore = y <= 6;
                break;
            case panel_locationType::BOTTOM:
                restore = y >= DockWindow::get_geometry().height - 6;
                break;
            case panel_locationType::LEFT:
                restore = x <= 6;
                break;
            case panel_locationType::RIGHT:
                restore = x >= DockWindow::get_geometry().width - 6;
                break;
        };
    }
    // g_print("restore %d\n",restore);
    if (!m_animate && !DockWindow::is_visible() && restore) {
        m_animate = true;
        m_easing_duration = 1.f;
    }

    if (!m_animate && DockWindow::is_visible() && !m_mouseIn && m_Timer.elapsed() > 2.5) {
        m_animate = true;
        m_easing_duration = 16.f;
        if (Configuration::is_activateStrut()) {
            DockWindow::updateStrut(1);
        }
    }

    if (m_animate) {
        m_Timer.stop();

        auto endTime = initTime + m_easing_duration;
        auto now = atime;
        int currentPositionX = 0;
        int currentPositionY = 0;
        int startPosition = 0;

        position = ofxeasing::map_clamp(now, initTime, endTime, 0,
                                        endPosition, &ofxeasing::linear::easeIn);

        switch (Configuration::get_dockWindowLocation())
        {
            case panel_locationType::TOP:
                if (DockWindow::is_visible()) {
                    startPosition = 0;
                    currentPositionY = startPosition - position;

                }
                else {
                    startPosition = -Configuration::get_dockWindowSize();
                    currentPositionY = startPosition + position;
                }

                break;
            case panel_locationType::BOTTOM:
                if (DockWindow::is_visible()) {
                    startPosition = (DockWindow::get_geometry().height - Configuration::get_dockWindowSize());
                    currentPositionY = startPosition + position;

                }
                else {
                    startPosition = (DockWindow::get_geometry().height);
                    currentPositionY = startPosition - position;
                }
                break;
            case panel_locationType::LEFT:
                if (DockWindow::is_visible()) {
                    startPosition = 0;
                    currentPositionX = startPosition - position;

                }
                else {
                    startPosition = -Configuration::get_dockWindowSize();
                    currentPositionX = startPosition + position;
                }

                break;

            case panel_locationType::RIGHT:
                if (DockWindow::is_visible()) {
                    startPosition = (DockWindow::get_geometry().width - Configuration::get_dockWindowSize());
                    currentPositionX = startPosition + position;

                }
                else {
                    startPosition = (DockWindow::get_geometry().width);
                    currentPositionX = startPosition - position;
                }
                break;
        };

        move(currentPositionX, currentPositionY);

        if (atime < m_easing_duration) {
            atime++;
        }

        if (abs(position) >= abs(endPosition)) {
            switch (Configuration::get_dockWindowLocation())
            {
                case panel_locationType::TOP:
                    DockWindow::set_visible(abs(currentPositionY) == 0);
                    break;
                case panel_locationType::BOTTOM:
                    DockWindow::set_visible(currentPositionY < DockWindow::get_geometry().height);
                    break;
                case panel_locationType::LEFT:
                    DockWindow::set_visible(abs(currentPositionX) == 0);
                    break;
                case panel_locationType::RIGHT:
                    DockWindow::set_visible(currentPositionX != DockWindow::get_geometry().width);
                    break;
            };

            if (Configuration::is_activateStrut()) {
                if (DockWindow::is_visible()) {
                    DockWindow::updateStrut();
                }
            }

            initTime = 0;
            atime = 0;
            m_animate = false;
        }
    }
    return true;
}

bool AppWindow::fullScreenTimer()
{
    if (Configuration::is_autoHide()) {
        return true;
    }

    m_isfullscreen = WindowControl::FullscreenActive();
    if (m_isfullscreen && !m_isfullscreenSet && DockWindow::is_visible()) {
        //   g_print("m_isfullscreen\n");
        DockWindow::hide();
        m_isfullscreenSet = true;

        return true;
    }

    if (!m_isfullscreen && m_isfullscreenSet && !DockWindow::is_visible()/* && !Configuration::is_autoHide()*/) {
        m_isfullscreenSet = false;
        //     g_print("normal\n");
        DockWindow::show();
        m_isfullscreenSet = false;
    }
    //g_print("check full sccreen\n");
    return true;
}

void AppWindow::monitor_size_changed_callback(GdkScreen *screen, gpointer gtkwindow)
{
    g_info("Monitor settings changed\n");
    // Adapt the monitor geometry and repositioning the window
    //  MonitorGeometry::update((Gtk::Window*)gtkwindow);

}

void AppWindow::on_active_window_changed_callback(WnckScreen *screen,
                                                  WnckWindow *previously_active_window, gpointer user_data)
{
    // m_currentMoveIndex = -1;

    // if (m_previewWindowActive || DockPanel::m_dragdropsStarts)
    //     return;

    WnckWindow *window = wnck_screen_get_active_window(screen);


    // g_print("\n");
    // g_print(wnck_window_get_name(window) );

}

void AppWindow::geometry_changed(WnckWindow *window)
{
    // if (wnck_window_is_maximized(window)) {
    g_print("A window has been maximized\n");
    // }
}

void AppWindow::window_opened_callback(WnckScreen* screen, WnckWindow* window, gpointer data){
    //g_print("open\n");
    // g_print(wnck_window_get_name(window) );

    //  g_signal_connect(window, "geometry-changed",
    //         G_CALLBACK(AppWindow::window_geometry_changed_callback), NULL);

    // if (wnck_window_is_maximized(window)) {
    //
    //    }
}

WnckWindow *fullscreen_window = NULL;

void AppWindow::window_geometry_changed_callback(WnckWindow *window, gpointer user_data)
{
    return;
    // if( wnck_window_is_fullscreen(window))
    g_print("geo change\n");




    m_isfullscreen = WindowControl::FullscreenActive();

    if (m_isfullscreen) {
        fullscreen_window = window;
        DockWindow::hide();
        return;
    }

    if (m_isfullscreen && window == fullscreen_window) {
        fullscreen_window = NULL;
        DockWindow::show();
    }

    return;

    if (m_isfullscreen && !m_isfullscreenSet) {

        g_print("m_isfullscreen\n");
        DockWindow::hide();

        m_isfullscreenSet = true;

        return;

    }

    if (!m_isfullscreen && m_isfullscreenSet) {
        //  m_isfullscreenSet=false;
        m_isfullscreenSet = false;
        //        if(!m_isfullscreenSet)
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
        m_isfullscreenSet = false;
    }

}

bool AppWindow::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    g_print("IN\n");
    m_mouseIn = true;
    return true;
}

bool AppWindow::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    g_print("OUT\n");
    m_mouseIn = false;
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


AppWindow::~AppWindow(){ }
