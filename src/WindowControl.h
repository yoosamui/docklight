/* 
 * File:   WindowControl.h
 * Author: yoo
 *
 * Created on November 6, 2018, 8:38 PM
 */

#ifndef WINDOWCONTROL_H
#define	WINDOWCONTROL_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm/window.h> 
//#include "DockItem.h"

#include <X11/X.h>

namespace WindowControl
{
bool FullscreenActive();
WnckWindow* get_ExistingWindowDock();
}

#endif	/* WINDOWCONTROL_H */

