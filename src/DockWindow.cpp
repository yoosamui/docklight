/* 
 * File:   MonitorGeometry.cpp
 * Author: yoo
 * 
 * Created on November 4, 2018, 12:45 PM
 */


#include <gdk/gdk.h>

#include "Configuration.h"
#include "DockWindow.h"
#include "WindowControl.h"
#include "AppWindow.h"
#include "Screen.h"
#include "Struts.h"

inline namespace DockWindow
{
    Gtk::Window* m_window = nullptr;
    bool m_visible;

    int init(Gtk::Window* window)
    {
        m_window = window;
        updateStrut();
        reSize(true);
        //     showDockWindow();
        return 0;
    }

    bool set_Visibility(bool visible)
    {
        m_visible = visible;
    };

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
        update(false);
    }

    void update(bool move)
    {
        //g_print("Update from dockpanel\n");
        if (m_window != nullptr) {
            reSize(move);
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
        reSize(false);
    }

    void reSize(bool forceMove)
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

                // calculate window size and position
                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsWidth() + get_dockWindowStartEndMargin();
                int startX = ((geometry.x + geometry.width) / 2) - (itemsSize / 2);
                
                // resize the window
                m_window->resize(itemsSize, areaSize);

                 // avoid show window when apps add or remove. 
                if (Configuration::is_autoHide() && !m_visible && !forceMove) {
                    int x, y;
                    m_window->get_position(x, y);
                    m_window->move(startX,y);
                    break;
                }
                
                if (location == panel_locationType::TOP) {
                    m_window->move(geometry.x + startX, geometry.y + Configuration::get_WindowDockMonitorMargin_Top());
                    m_visible = true;
                    break;
                }
                
                 areaSize += Configuration::get_WindowDockMonitorMargin_Bottom();
                 m_window->move(geometry.x + startX, (geometry.y + geometry.height) - areaSize);
                 m_visible = true;
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
                    m_visible = true;
                    break;
                }

                // calculate window size and position
                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsHeight() + get_dockWindowStartEndMargin();
                int startY = (geometry.height / 2) - (itemsSize / 2);
                areaSize += Configuration::get_WindowDockMonitorMargin_Right();
                int posx = (geometry.x + geometry.width) - areaSize;
                int posy = geometry.y + startY;

                // resize the window
                m_window->resize(areaSize, itemsSize);

                // avoid show window when apps add in. 
                if (Configuration::is_autoHide() && !m_visible && !forceMove) {
                    int x, y;
                    m_window->get_position(x, y);
                    m_window->move(x, posy);
                    break;
                }

                if (location == panel_locationType::LEFT) {
                    m_window->move(geometry.x + Configuration::get_WindowDockMonitorMargin_Left(), geometry.y + startY);
                    m_visible = true;
                    break;
                }

                m_window->move(posx, posy);
                m_visible = true;

                break;
            }
        }
    }
}
