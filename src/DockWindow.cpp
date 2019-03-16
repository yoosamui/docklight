/* 
 * File:   MonitorGeometry.cpp
 * Author: yoo
 * 
 * Created on November 4, 2018, 12:45 PM
 */

#include "Configuration.h"


//#include <gtk-3.0/gdk/gdkdisplay.h>
#include <gdk/gdk.h>
//#include <X11/Xlib.h> 

#include "DockWindow.h"
#include "WindowControl.h"
#include "AppWindow.h"
#include "Screen.h"
#include "Struts.h"

inline namespace DockWindow
{
    Gtk::Window* m_window = nullptr;

    int init(Gtk::Window* window)
    {
        m_window = window;
        updateStrut();
        showDockWindow();
        return 0;
    }

    int get_DockWindowWidth()
    {
        if (m_window != nullptr)
            return m_window->get_width();
    }

    int get_DockWindowHeight()
    {
        if (m_window != nullptr)
            return m_window->get_height();
    }

    guint get_dockWindowStartEndMargin()
    {
        return 40;
    }

    void update()
    {
        //g_print("Update from dockpanel\n");
        if (m_window != nullptr) {
            reSize();
        }
    }

    GdkRectangle get_geometry()
    {
        if (m_window != nullptr) {
            return ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->geometry;
        }
    }

    GdkRectangle get_workarea()
    {
        if (m_window != nullptr) {
            return ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->workarea;
        }
    }

    void updateGeometry()
    {
        if (m_window != nullptr)
            ((AppWindow*)m_window)->m_screen.update();
    }

    void updateStrut()
    {
        if (Configuration::is_activateStrut()) {
            if (m_window != nullptr)
                ((AppWindow*)m_window)->m_screen.struts->update();
        }
    }

    void removeStrut()
    {
        if (Configuration::is_activateStrut()) {
            if (m_window != nullptr)
                ((AppWindow*)m_window)->m_screen.struts->remove();
        }
    }

    gint getClientSize()
    {
        if (m_window == nullptr) {
            return 0;
        }

        int width = 0, height = 0;
        m_window->get_size(width, height);

        switch (Configuration::get_dockWindowLocation())
        {
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
                return height;
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
                return width;
        }

        return 0;
    }

    void reSize()
    {
        if (m_window == nullptr) {
            return;
        }

        g_print("Resize\n");
        auto geometry = ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->geometry;
        auto areaSize = Configuration::get_dockWindowSize();
        auto location = Configuration::get_dockWindowLocation();
        switch (location)
        {
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(geometry.width, areaSize);

                    if (location == panel_locationType::TOP) {
                        m_window->move(geometry.x, geometry.y + Configuration::get_WindowDockMonitorMargin_Top());
                        break;
                    }

                    m_window->move(geometry.x, (geometry.y + geometry.height)
                                   - (areaSize + Configuration::get_WindowDockMonitorMargin_Bottom()));

                    break;
                }

                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsWidth() + get_dockWindowStartEndMargin();

                int center = (/*(geometry.x +*/ geometry.width / 2);
                int startX = center - (itemsSize / 2);

                m_window->resize(itemsSize, areaSize);

                if (location == panel_locationType::TOP) {
                    // m_window->move(workarea.x + startX, workarea.y);
                    m_window->move(geometry.x + startX, geometry.y + Configuration::get_WindowDockMonitorMargin_Top());
                }
                else {

                    areaSize += Configuration::get_WindowDockMonitorMargin_Bottom();
                    m_window->move(geometry.x + startX, (geometry.y + geometry.height) - areaSize);

                }


                break;
            }
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
            {
                if (Configuration::is_panelMode()) {
                    if (location == panel_locationType::LEFT) {
                        m_window->resize(areaSize, geometry.height);
                    //    m_window->move(geometry.x + Configuration::get_WindowDockMonitorMargin_Left(), geometry.y);
                        break;
                    }

                    m_window->resize(areaSize, geometry.height);
               //     m_window->move((geometry.x + geometry.width) - areaSize, geometry.y);
                    break;
                }

                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsHeight() + get_dockWindowStartEndMargin();

                int center = (geometry.height / 2);
                int startY = center - (itemsSize / 2);

                m_window->resize(areaSize, itemsSize);

                if (location == panel_locationType::LEFT) {
               //     m_window->move(geometry.x + Configuration::get_WindowDockMonitorMargin_Left(), geometry.y + startY);
                }
                else {
                    areaSize += Configuration::get_WindowDockMonitorMargin_Right();

                  //  m_window->move((geometry.x + geometry.width) - areaSize, geometry.y + startY);
                }


                break;
            }
        }

    }

    void showDockWindow()
    {
        if (m_window == nullptr) {
            return;
        }

        g_print("showDockWindow\n");
        auto geometry = ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->geometry;
        auto areaSize = Configuration::get_dockWindowSize();
        auto location = Configuration::get_dockWindowLocation();
        switch (location)
        {
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(geometry.width, areaSize);

                    if (location == panel_locationType::TOP) {
                        m_window->move(geometry.x, geometry.y + Configuration::get_WindowDockMonitorMargin_Top());
                        break;
                    }

                    m_window->move(geometry.x, (geometry.y + geometry.height)
                                   - (areaSize + Configuration::get_WindowDockMonitorMargin_Bottom()));

                    break;
                }

                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsWidth() + get_dockWindowStartEndMargin();

                int center = (/*(geometry.x +*/ geometry.width / 2);
                int startX = center - (itemsSize / 2);

                m_window->resize(itemsSize, areaSize);

                if (location == panel_locationType::TOP) {
                    // m_window->move(workarea.x + startX, workarea.y);
                    m_window->move(geometry.x + startX, geometry.y + Configuration::get_WindowDockMonitorMargin_Top());
                }
                else {

                    areaSize += Configuration::get_WindowDockMonitorMargin_Bottom();
                    m_window->move(geometry.x + startX, (geometry.y + geometry.height) - areaSize);

                }


                break;
            }
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
            {
                if (Configuration::is_panelMode()) {
                    if (location == panel_locationType::LEFT) {
                        m_window->resize(areaSize, geometry.height);
                        m_window->move(geometry.x + Configuration::get_WindowDockMonitorMargin_Left(), geometry.y);
                        break;
                    }

                    m_window->resize(areaSize, geometry.height);
                    m_window->move((geometry.x + geometry.width) - areaSize, geometry.y);
                    break;
                }

                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsHeight() + get_dockWindowStartEndMargin();

                int center = (geometry.height / 2);
                int startY = center - (itemsSize / 2);

                m_window->resize(areaSize, itemsSize);

                if (location == panel_locationType::LEFT) {
                    m_window->move(geometry.x + Configuration::get_WindowDockMonitorMargin_Left(), geometry.y + startY);
                }
                else {
                    areaSize += Configuration::get_WindowDockMonitorMargin_Right();

                    m_window->move((geometry.x + geometry.width) - areaSize, geometry.y + startY);
                }


                break;
            }
        }

    }
}
