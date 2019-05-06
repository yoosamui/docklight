/*
 * File:   Struts.cpp
 * Author: yoo
 *
 * Created on January 5, 2019, 4:43 PM
 */

#include "Struts.h"
#include "Screen.h"
#include "DockWindow.h"

Struts::Struts(Gtk::Window* window, Screen* screen)
{
    this->window = window;
    this->screen = screen;
}

Struts::~Struts(){ }

void Struts::remove()
{
    this->update(true);
}

void Struts::update()
{
    this->update(false);
}

void Struts::update(bool reset)
{
    long insets[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(window->gobj()));
    auto gdk_window = gtk_widget_get_window(toplevel);
    if (gdk_window == NULL) {
        g_critical("update: gdk_window is null.");
        return;
    }
    if (!reset) {
        this->screen->update();
        guint areaSize = DockWindow::reSize();//DockWindow::getClientSize(); //Configuration::get_dockWindowSize();
        auto primary = this->screen->get_PrimaryMonitor();
        auto next = this->screen->get_NextMonitor();

        if(next == NULL)
        {
            next = primary;
        }

        switch (Configuration::get_dockWindowLocation())
        {
            case panel_locationType::TOP:
            {
                //if (!Configuration::is_panelMode()) {
                    //areaSize += Configuration::get_WindowDockMonitorMargin_Top();
                //}
              if (this->screen->get_MonitorsCount() > 1) {
                    //0,0,410,0,0,0,0,0,2560,4479,0,0
                    if (primary->geometry.x > 0) {
                        insets[strutsPosition::Top] = primary->geometry.y + areaSize +
                                Configuration::get_WindowDockMonitorMargin_Top();
                        insets[strutsPosition::TopStart] = next->geometry.width;
                        insets[strutsPosition::TopEnd] = (primary->geometry.width + next->geometry.width) - 1;
                        break;
                    }
                }

                // ONE MONITOR xprop -name "Desktop" -f _NET_WM_STRUT_PARTIAL 32c -set _NET_WM_STRUT_PARTIAL 0,0,56,0,0,0,0,0,0,1920,0,0 OK!
                insets[strutsPosition::Top] = areaSize;
                insets[strutsPosition::TopStart] =  0;
                insets[strutsPosition::TopEnd] = primary->geometry.width;


                break;
            }


            case panel_locationType::BOTTOM:
            {
                //if (!Configuration::is_panelMode()) {
                    //areaSize += Configuration::get_WindowDockMonitorMargin_Bottom();
                //}

                if (this->screen->get_MonitorsCount() > 1) {
                    //0,0,0,50,0,0,0,0,0,0,2560,4479
                    if (primary->geometry.x > 0) {
                        insets[strutsPosition::Bottom] = areaSize ;//+ primary->workarea.y;
                        insets[strutsPosition::BottomStart] = next->geometry.width;
                        insets[strutsPosition::BottomEnd] = (primary->geometry.width + next->geometry.width) - 1;
                        break;
                    }
                }


                // ONE MONITOR xprop -name "Desktop" -f _NET_WM_STRUT_PARTIAL 32c -set _NET_WM_STRUT_PARTIAL 0,0,0,56,0,0,0,0,0,0,0,1920 OK!
                insets[strutsPosition::Bottom] = areaSize;                           // strut size;
                insets[strutsPosition::BottomStart] = 0;                             // dock start pos
                insets[strutsPosition::BottomEnd] = primary->geometry.width;         // dock end pos

                break;
            }

            case panel_locationType::LEFT:
            {
                //if (!Configuration::is_panelMode()) {
                 //areaSize += Configuration::get_WindowDockMonitorMargin_Left();
                 //}

                if (this->screen->get_MonitorsCount() > 1) {

                    //0,0,2560,0,0,0,0,0,2560,2600,0,0
                    if (primary->geometry.x > 0) {
                        insets[strutsPosition::Top] = next->geometry.width - 1;
                        insets[strutsPosition::TopStart] = next->geometry.width - 1;
                        insets[strutsPosition::TopEnd] = insets[strutsPosition::TopStart] + areaSize;
                        break;
                    }
                }

                insets[strutsPosition::Left] = areaSize;
                insets[strutsPosition::LeftStart] = primary->geometry.x - 1;
                insets[strutsPosition::LeftEnd] = primary->geometry.x + primary->geometry.height;


                break;

            }

            case panel_locationType::RIGHT:
            {

                //if (!Configuration::is_panelMode()) {
                //areaSize += Configuration::get_WindowDockMonitorMargin_Right();
                //}
                if (this->screen->get_MonitorsCount() > 1) {
                    // 0,60,0,0,0,0,0,1080,0,0,0,0
                    if (primary->geometry.x > 0) {
                        insets[strutsPosition::Right] = areaSize;
                        insets[strutsPosition::RightStart] = 0;
                        insets[strutsPosition::RightEnd] = next->geometry.width - 1;
                        break;
                    }
                }

                // at this point this is the firts horizotal monitor
                if (next->geometry.x == 0) {
                    //0,60,0,0,0,0,0,1080,0,0,0,0
                    insets[strutsPosition::Right] = areaSize;
                    insets[strutsPosition::RightStart] = 0;
                    insets[strutsPosition::RightEnd] = next->geometry.height - 1;
                    break;
                }

                //0,0,2560,0,0,0,0,0,2500,2550,0,0
                insets[strutsPosition::Top] = primary->geometry.width -1 ;
                insets[strutsPosition::TopStart] = primary->geometry.width - areaSize ;
                insets[strutsPosition::TopEnd] = insets[strutsPosition::TopStart] + areaSize ;
                break;
            }
        }
    }

    gdk_property_change(gdk_window,
                        gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                        gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                        (unsigned char *)&insets, 12);


    gdk_property_change(gdk_window,
                        gdk_atom_intern("_NET_WM_STRUT", FALSE),
                        gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                        (unsigned char *)&insets, 4);

}
