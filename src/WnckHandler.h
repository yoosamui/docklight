/* 
 * File:   WckHandler.h
 * Author: yoo
 *
 * Created on March 18, 2019, 6:01 PM
 */

#ifndef WNCKHANDLER_H
#define	WNCKHANDLER_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm/window.h> 
#include "DockItem.h"

#include <X11/X.h>

namespace WnckHandler
{
    void ActivateWindow(WnckWindow* window); 
    WnckWindow* get_ActiveWindowIfAny(DockItem* item);
}

#endif	/* WNCKHANDLER_H */

