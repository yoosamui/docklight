/* 
 * File:   Struts.h
 * Author: yoo
 *
 * Created on January 5, 2019, 4:43 PM
 */

#ifndef STRUTS_H
#define	STRUTS_H

#include <gtkmm/window.h>
#include <gdkmm/screen.h>
#include <gdk/gdk.h>
#include "Configuration.h"


class Screen;

class Struts {
public:
    Struts(Gtk::Window* window, Screen* screen);
    virtual ~Struts();
    void update();
    void remove();

    
private:
    
     Gtk::Window* window = NULL;
     Screen* screen;
     
     void update(bool reset);
    
};

#endif	/* STRUTS_H */

