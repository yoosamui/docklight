/* 
 * File:   AppRunAnimation.h
 * Author: yoo
 *
 * Created on March 21, 2019, 6:00 PM
 */

#ifndef APPRUNTHREAD_H
#define	APPRUNTHREAD_H

#include <gtkmm.h> 
#include <thread>


class DockPanel;

class AppRunAnimation {
public:
    AppRunAnimation();
    virtual ~AppRunAnimation();
    void start(DockPanel& panel,  Glib::RefPtr<Gdk::Pixbuf> image);
};

#endif	/* APPRUNTHREAD_H */

