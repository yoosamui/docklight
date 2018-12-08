/* 
 * File:   AppWindow.h
 * Author: yoo
 *
 * Created on November 3, 2018, 2:45 PM
 */

#ifndef APPWINDOW_H
#define	APPWINDOW_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <gtkmm/window.h>
#include <gdkmm/screen.h>

#include "DockPanel.h"

class AppWindow : public Gtk::Window {
public:
    AppWindow();
    int init();
    virtual ~AppWindow();
    DockPanel m_dockpanel;

    void update();
    
private:


    float m_easing_duration;
    static void monitor_size_changed_callback(GdkScreen *screen, gpointer gtkwindow);
    static void window_geometry_changed_callback(WnckWindow *window, gpointer user_data);
    static void geometry_changed(WnckWindow *window);
    bool fullscreenActive();

    static void window_opened_callback(WnckScreen* screen, WnckWindow* window, gpointer data);

    static void configureCallback(GtkWindow* parentWindow, GdkEvent* event, gpointer data);

    static void LeaveFunc(GtkWidget *widget, GdkEvent *event, gpointer callback_data);
    static void on_active_window_changed_callback(WnckScreen* screen,
            WnckWindow* previously_active_window, gpointer user_data);

    //  static void window_geometry_changed_callback(WnckWindow *window, gpointer user_data);
    Glib::Timer m_Timer;

    bool autoHideTimer();
    bool fullScreenTimer();
    //  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    // bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    //        m_dockpanel.on_draw(cr);
    //    }

    virtual bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    virtual bool on_leave_notify_event(GdkEventCrossing* crossing_event);

    static bool m_isfullscreenSet;
    static bool m_isfullscreen;
    bool m_mouseIn = false;
    bool m_animate = false;
    bool m_timerStoped = true;
    float position = 0.f;
    float initTime = 0.f;
    float endPosition = 0.f;
    float atime = 0.f;

};

#endif	/* APPWINDOW_H */

