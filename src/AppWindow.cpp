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
#include <glibmm-2.4/glibmm/timer.h>
#include "AppWindow.h"
#include "../config.h"
#include "DockWindowHandler.h"
#include "WindowControl.h"
#include "Animations.h"
#include "Utilities.h"

bool AppWindow::m_isfullscreenSet;
bool AppWindow::m_isfullscreen;

static const gchar* type_name(WnckWindowType type)
{
    switch (type)
    {
        case WNCK_WINDOW_NORMAL: return "normal";
        case WNCK_WINDOW_DESKTOP: return "desktop";
        case WNCK_WINDOW_DOCK: return "dock";
        case WNCK_WINDOW_DIALOG: return "dialog";
        case WNCK_WINDOW_TOOLBAR: return "toolbar";
        case WNCK_WINDOW_MENU: return "menu";
        case WNCK_WINDOW_UTILITY: return "utility";
        case WNCK_WINDOW_SPLASHSCREEN: return "splashscreen";
        default: return "UNKNOWN";
    }
}

AppWindow::AppWindow()
{
    //    add_events(Gdk::BUTTON_PRESS_MASK);
    //    set_app_paintable(true);

    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

}

//void AppWindow::LeaveFunc(GtkWidget *widget, GdkEvent *event, gpointer callback_data)
//{
//    // if (event==(GdkEvent *)NULL || event->type!=GDK_LEAVE_NOTIFY) {
//    // ErrorFunction( "LeaveFunc: NULL event or wrong type\n" );
//    // return; /* bogus event */
//    // }
//    /* event is good */
//
//
//   
//    exit(1);
//
//}

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


    //this->resize(300, 1000); //debug

    // Add the dock panel
    this->add(m_dockpanel);

    //this->add(dockBckPanel);
    //void Gtk::Window::set_transient_for (Window& parent)
    //  this->set_transient_for(&dockBckPanel);
    //    

    //https://developer.gnome.org/gtk-tutorial/stable/x2431.html
    this->add_events(
                     Gdk::PROPERTY_CHANGE_MASK |
                     Gdk::POINTER_MOTION_HINT_MASK |
                     Gdk::ENTER_NOTIFY_MASK |
                     Gdk::LEAVE_NOTIFY_MASK |
                     Gdk::POINTER_MOTION_MASK |
                     Gdk::STRUCTURE_MASK |
                     Gdk::PROPERTY_CHANGE_MASK);


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
GDK_PROXIMITY_IN_MASK | GDK_PROXIMITY_OUT_MASK | GDK_SUBSTRUCTURE_MASK 
GDK_SCROLL_MASK | GDK_TOUCH_MASK | GDK_SMOOTH_SCROLL_MASK );
     */





    //
    GdkScreen *screen = gdk_screen_get_default();
    WnckScreen *wnckscreen = wnck_screen_get(0);
    //
    //    // needed for "configure-event"
    //    this->add_events(Gdk::STRUCTURE_MASK);
    //
    //    // The monitors-changed signal is emitted when the number,
    //    // size or position of the monitors attached to the screen change.
    g_signal_connect(G_OBJECT(screen), "monitors-changed",
                     G_CALLBACK(monitor_size_changed_callback),
                     (gpointer) this);


    //  g_signal_connect(G_OBJECT(wnckscreen), "property_change",
    //                 G_CALLBACK(AppWindow::LeaveFunc), NULL);
    /*
        g_signal_connect(wnckscreen, "active_window_changed",
                         G_CALLBACK(AppWindow::on_active_window_changed_callback), NULL);



        g_signal_connect(G_OBJECT(wnckscreen), "window_opened",
                         G_CALLBACK(window_opened_callback), NULL);

        g_signal_connect(G_OBJECT(wnckscreen), "window_opened",
                         G_CALLBACK(window_opened_callback), NULL);

     */

    // Launch timer every second
    // Glib::signal_timeout().connect(sigc::mem_fun(*this,&AppWindow::on_timeoutDraw), 1000);

    //   g_signal_connect(window, "geometry-changed",
    //                 G_CALLBACK(geometry_changed), NULL);
    // wnck_
    // Initialize the monitor geometry.
    //https://lazka.github.io/pgi-docs/Wnck-3.0/classes/Window.html

    this->show_all();


    if (DockWindow::init(this) != 0)
        return -1;








    m_dockpanel.preInit(this);
    //    // repositioning10 the window
    //    if (DockWindow::updateStrut() != 0)
    //        return -1;

    //Glib::signal_timeout().connect(sigc::mem_fun(*this, &AppWindow::fullScreenTimer), 100);
    //Glib::signal_timeout().connect(sigc::mem_fun(*this, &AppWindow::autoHideTimer), DEF_FRAMERATE);

    initTime = 0; //DockWindow::get_geometry().height-100;;
    // endPosition = DockWindow::get_geometry().height ;//- get_height();//Configuration::get_dockWindowSize();//+180);//DockWindow::get_geometry().height; //this->
    //1080
    endPosition = Configuration::get_dockWindowSize(); //+180);//DockWindow::get_geometry().height; //this->
    // g_print("%f\n", endPosition);

    //time=10;
    /*
        // test
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);
        char* buf[512];
        for (window_l = wnck_screen_get_windows(screen);
             window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;
            // char *buf[512];

            if (g_strcmp0(wnck_window_get_class_instance_name(window), "")) {
                //   auto instanceName = wnck_window_get_class_instance_name (window);

                g_print("\nname=%s\niconname=%s\nclassgroup=%s\nclassinstance=%s\ntype=%s\n",
                        wnck_window_get_name(window),
                        wnck_window_get_icon_name(window),
                        wnck_window_get_class_group_name(window),
                        wnck_window_get_class_instance_name(window),
                        type_name(wnck_window_get_window_type(window)));
            }

            // Translators: A class is like a "family". E.g., all gvim windows are of the
            //  same class. The class instance is a way to differentiate windows belonging
            //  to the same class group. 
           //  /
            // g_print (_("Class Instance: %s\n"), buf);

            g_print("XID: %lu\n", wnck_window_get_xid(window));

            //auto& instanceName = wnck_window_get_class_instance_name(window);
            //this->set_gravity(Gdk::GRAVITY_SOUTH_WEST);//.gdk.GRAVITY_SOUTH_EAST);
        }
     */

    if (Configuration::is_activateStrut()) {
        DockWindow::updateStrut();
    }
    
    DockWindow::show();
    return 0;
}
//bool visible = true;

void AppWindow::update(bool mode)
{
   // DockWindow::updateGeometry();
    DockWindow::show();
    //DockWindow::updateStrut();

    g_print("--------------------------updated\n");

    //DockWindow::show();

    return;

    //   
    //    
    //    if (DockWindow::is_visible()) {
    //        DockWindow::show();
    //    }
    //    else {
    //        DockWindow::hide();
    //    }

}

/*
bool AppWindow::on_timeoutDraw()
{
    Gtk::Widget::queue_draw();
    return true;
}


bool AppWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
 // DockWindow Color

//    if (Configuration::is_panelMode()) {
//        cr->set_source_rgba(0.0, 0.0, 0.8, 0.4); // partially translucent
//        cr->paint();
//    }
//    else {
//        cr->set_source_rgba(0.0, 0.0, 175.8, 0.4); // partially translucent
//        RoundedRectangle(cr, 0, 0, DockWindow::getDockWindowWidth(), DockWindow::getDockWindowHeight(), 6);
//        cr->fill();
//    }
   cr->set_source_rgba(0.0, 0.0, 0.8, 0.4); // partially translucent
   cr->paint();
    
    return true;
}
 */

bool AppWindow::autoHideTimer()
{
    return true;
    if (Configuration::is_autoHide() == false /*|| m_isfullscreenSet == true*/) {
        return true;
    }

    if (!m_animate && !m_mouseIn && DockWindow::is_visible() && m_timerStoped) {
        //  m_Timer.reset();
        m_Timer.start();
        m_timerStoped = false;

        // return true;
    }

    bool restore = false;
    int x, y;
    Utilities::getMousePosition(x, y);
    if (!m_animate && !m_mouseIn && !DockWindow::is_visible()) {
        switch (Configuration::get_dockWindowLocation())
        {
            case panel_locationType::TOP:
                restore = y <= 6;
                break;
            case panel_locationType::BOTTOM:
                restore = y >= DockWindow::get_geometry().height - 6;
                // m_mouseIn = true;
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
        m_easing_duration = 4.f;
        // this->show();
        g_print("Restore\n");
        //  DockWindow::set_visible(true);
    }

    if (!m_animate && DockWindow::is_visible() && !m_mouseIn && m_Timer.elapsed() > 2.0) {
        m_Timer.stop();

        m_animate = true;
        m_easing_duration = 6.f;
        if (Configuration::is_activateStrut()) {
            DockWindow::updateStrut(1);
        }

        // m_Timer.reset();
        g_print("Hide\n");
    }

    if (m_animate) {

        this->show();

        auto endTime = initTime + m_easing_duration;
        auto now = atime;
        int currentPositionX = 0;
        int currentPositionY = 0;
        float startPosition = 0.f;

        position = ofxeasing::map_clamp(now, initTime, endTime, 0,
                                        endPosition, &ofxeasing::linear::easeIn);

        switch (Configuration::get_dockWindowLocation())
        {
            case panel_locationType::TOP:
                if (DockWindow::is_visible()) {
                    startPosition = 0;
                    currentPositionY = startPosition - position;

                }
                else {//show
                    startPosition = (int)-Configuration::get_dockWindowSize();
                    currentPositionY = startPosition + position;
                }

                break;
            case panel_locationType::BOTTOM:
                if (DockWindow::is_visible()) {
                    startPosition = (DockWindow::get_geometry().height -
                            Configuration::get_dockWindowSize());
                    currentPositionY = startPosition + position;

                    //   g_print("hide pos: %f %f\n",currentPositionY, position);
                    //  this->move(currentPositionX, currentPositionY );
                }
                else {
                    //   DockWindow::set_visible(true);
                    //show
                    startPosition = (int)DockWindow::get_geometry().height;
                    currentPositionY = (int)(startPosition - position);
                    // currentPositionX=200;
                    //                    int xx,yy;
                    //                    this->get_position(xx,yy);
                    //                    
                    //                           g_print("winpos%d %d\n",xx, yy);

                }
                break;
            case panel_locationType::LEFT:
                if (DockWindow::is_visible()) {
                    //hide
                    startPosition = (int)DockWindow::get_geometry().x;
                    currentPositionX = startPosition - position;

                }
                else { // show
                    startPosition = (int)(DockWindow::get_geometry().x - Configuration::get_dockWindowSize());
                    currentPositionX = startPosition + position;
                }

                break;

            case panel_locationType::RIGHT:
                if (DockWindow::is_visible()) {
                    startPosition = (DockWindow::get_geometry().width -
                            Configuration::get_dockWindowSize());
                    currentPositionX = startPosition + position;

                }
                else {
                    startPosition = (DockWindow::get_geometry().width);
                    currentPositionX = startPosition - position;
                }
                break;
        };

        // this->move(currentPositionX, currentPositionY );
        // this->show();
        //   gtk_window_move(*this, currentPositionX, currentPositionY);
        DockWindow::move(currentPositionX, currentPositionY);
        g_print("start pos: %d %d\n", currentPositionX, currentPositionY);

        if (atime < m_easing_duration) {
            atime++;
        }

        // if (abs(position) >= abs(endPosition))
        if (position >= endPosition) {

            switch (Configuration::get_dockWindowLocation())
            {
                case panel_locationType::TOP:
                    DockWindow::set_visible(abs(currentPositionY) == 0);
                    break;
                case panel_locationType::BOTTOM:
                    // DockWindow::set_visible(currentPositionY < DockWindow::get_geometry().height);
                    DockWindow::set_visible(currentPositionY == DockWindow::get_geometry().height - Configuration::get_dockWindowSize());
                    break;
                case panel_locationType::LEFT:
                    DockWindow::set_visible(currentPositionX == DockWindow::get_geometry().x);
                    break;
                case panel_locationType::RIGHT:
                    DockWindow::set_visible(currentPositionX != DockWindow::get_geometry().width);
                    break;
            };

            //            if (Configuration::is_activateStrut()) {
            //                if (DockWindow::is_visible()) {
            //                    DockWindow::updateStrut();
            //                }
            //            }

            //                    if (DockWindow::is_visible())
            //                     this->hide();{
            //                        this->hide();
            //                    }
            //                    else
            //                    {
            //                        this->show();
            //                    
            //                    }

            initTime = 0;
            atime = 0;
            m_animate = false;
            m_timerStoped = true;
            //  m_Timer.reset();
            //  m_Timer.start();

            g_print("END\n");
            //  m_Timer.reset();

        }
    }
    return true;
}

bool AppWindow::fullScreenTimer()
{
    return true;
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
    g_print("...............Monitor settings changed\n");
    // Adapt the monitor geometry and repositioning the window
    //  MonitorGeometry::update((Gtk::Window*)gtkwindow);

    DockWindow::resetStrut();
    DockWindow::updateGeometry();
   
    DockWindow::show();
   // DockWindow::updateStrut();
   
    

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
    // g_print("IN\n");
    m_mouseIn = true;
    return true;
}

bool AppWindow::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    //  g_print("OUT\n");
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
