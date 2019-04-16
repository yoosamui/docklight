//*****************************************************************
//
//  Copyright (C) Juan R. Gonzalez
//  Created on November 20, 2018 
//  j-gonzalez@email.de
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
//****************************************************************
#include <gdk/gdk.h>

#include "Configuration.h"
#include "DockWindow.h"
#include "WindowControl.h"
#include "AppWindow.h"
#include "Screen.h"
#include "Struts.h"
#include "DockItemPositions.h"

namespace DockWindow
{
    Gtk::Window* m_window = nullptr;
    bool m_visible;

    /**
     * Update struts and resizes the window.
     * @param: the main window.
     * @return: 0 = success.
     */
    int init(Gtk::Window* window)
    {
        m_window = window;
        Monitor::updateStrut();
        reSize(true);
        return 0;
    }

    /**
     * Determine the current dock panel location.
     * @return: the location enum.
     */
    bool is_Horizontal()
    {
        switch (Configuration::get_dockWindowLocation()){
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
                return true;
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
                return false;
        }
    }

    
    void set_Visibility(bool visible)
    {
        m_visible = visible;
    };

    /**
     * Get the visibility flag.
     * @return true if the window is visible otherwise false.
     */
    bool is_Visible()
    {
        return m_visible;
    }

    /**
     * returns the window width.
     * @return the width.
     */
    int get_DockWindowWidth()
    {
        if (m_window != nullptr)
            return m_window->get_width();
    }

    /**
     * returns the window height.
     * @return the height.
     */
    int get_DockWindowHeight()
    {
        if (m_window != nullptr)
            return m_window->get_height();
    }

    /**
     * sets the window position to the parameters.
     */
    void get_DockWindowPosition(int& x, int& y)
    {
        x = y = 0;
        if (m_window != nullptr) {
            m_window->get_position(x, y);
        }
    }

    /**
     * !!!WARNING!!!
     * minimal value must be greater then 1. Division by 2 should never be 0.
     */
    guint get_dockWindowStartEndMargin()
    {
        return 20;
    }

    /**
     * Resizes the dock window. 
     */
    void update()
    {
        update(false);
    }

    /**
     * Resizes ans/or moves the dock window.
     * @param: the move flag. If is true the window will resized and move.
     */
    void update(bool move)
    {
        //g_print("Update from dockpanel\n");
        if (m_window != nullptr) {
            reSize(move);
        }
    }

    /**
     * Resize the docck window.
     * @return: the size.
     */
    int reSize() 
    {
        return reSize(false);
    }

    /**
     * Count the items and calculate the size of the window.
     * Resizes the window and positioning it.      
     * @param forceMove
     * @return the ClietSize
     */
    int reSize(bool forceMove)
    {
        if (m_window == nullptr || Configuration::is_allowDraw() == false) {
            return 0;
        }

        g_print("Resize\n");
        auto geometry = ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->geometry;
        auto areaSize = Configuration::get_dockWindowSize();
        auto location = Configuration::get_dockWindowLocation();

        if (is_Horizontal())
        {
            if (Configuration::is_panelMode()) {

                m_window->resize(geometry.width, areaSize);
                if (Configuration::is_autoHide() && !m_visible && !forceMove) {
                    return areaSize;
                }
                if (location == panel_locationType::TOP) {
                    m_window->move(geometry.x, geometry.y);
                    return areaSize;
                }

                m_window->move(geometry.x, (geometry.y + geometry.height) - areaSize);
                return areaSize;
            }


            int itemsSize = DockItemPositions::get_dockItemsWidth();
            int startX = ((geometry.x + geometry.width) / 2) - (itemsSize / 2);

            // resize the window
            m_window->resize(itemsSize, areaSize);
            if (Configuration::is_autoHide() && !m_visible && !forceMove) {
                return areaSize;
            }

            if (location == panel_locationType::TOP) {
                m_window->move(geometry.x + startX, geometry.y + Configuration::get_WindowDockMonitorMargin_Top());
                m_visible = true;
                return areaSize;
            }

            areaSize += Configuration::get_WindowDockMonitorMargin_Bottom();
            m_window->move(geometry.x + startX, (geometry.y + geometry.height) - areaSize);
            m_visible = true;
            return areaSize;
        }
        else
        {
            if (Configuration::is_panelMode()) {
                if (location == panel_locationType::LEFT) {
                    m_window->resize(areaSize, geometry.height);
                    if (Configuration::is_autoHide() && !m_visible && !forceMove) {
                        return areaSize;
                    }
                    m_window->move(geometry.x + Configuration::get_WindowDockMonitorMargin_Left(), geometry.y);
                    return areaSize;
                }

                m_window->resize(areaSize, geometry.height);
                if (Configuration::is_autoHide() && !m_visible && !forceMove) {
                    return areaSize;
                }
                //m_window->move((geometry.x + geometry.width) - (areaSize + Configuration::get_WindowDockMonitorMargin_Right()), geometry.y);
                areaSize += Configuration::get_WindowDockMonitorMargin_Right();
                m_window->move((geometry.x + geometry.width) - areaSize, geometry.y);
                m_visible = true;
                return areaSize;
            }

            // calculate window size and position
            int itemsSize = DockItemPositions::get_dockItemsHeight();
            g_print("DockItemHeight %d\n", itemsSize);
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
                return areaSize;
            }

            if (location == panel_locationType::LEFT) {
                m_window->move(geometry.x + Configuration::get_WindowDockMonitorMargin_Left(), geometry.y + startY);
                m_visible = true;
                return areaSize;
            }

            m_window->move(posx - Configuration::get_WindowDockMonitorMargin_Right(), posy);
            m_visible = true;

            return areaSize;
        }


        return areaSize;
    }

    /**
     * Hide the window;
     */
    void hide()
    {
        if (m_window == nullptr) {
            return;
        }

        g_print("Hide\n");
        auto geometry = ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->geometry;
        auto areaSize = Configuration::get_dockWindowSize();
        auto location = Configuration::get_dockWindowLocation();

        int x, y;
        m_window->get_position(x, y);

        if (is_Horizontal()){

            if (location == panel_locationType::TOP) {
                m_window->move(x, geometry.y - (Configuration::get_dockWindowSize() - 1));
                m_visible = false;
                return;
            }

            m_window->move(x, DockWindow::Monitor::get_geometry().height + Configuration::get_dockWindowSize() - 1);
            m_visible = false;
        }
        else{

            if (location == panel_locationType::LEFT) {
                areaSize += Configuration::get_WindowDockMonitorMargin_Left();
                m_window->move(geometry.x - (areaSize + 4), y);
                m_visible = false;
                return;
            }

            areaSize += Configuration::get_WindowDockMonitorMargin_Right();
            m_window->move(DockWindow::Monitor::get_geometry().width + (areaSize - 4), y);
            m_visible = false;

        }
    }

    namespace Monitor
    {
        /**
         * Returns the monitor geometry.
         */
        GdkRectangle get_geometry()
        {
            if (m_window != nullptr) {
                return ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->geometry;
            }
        }

        /**
         * Returns the monitor working area
         */
        GdkRectangle get_workarea()
        {
            if (m_window != nullptr) {
                return ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->workarea;
            }
        }

        /*
         * Updates the monitor geometry
         */
        void updateGeometry()
        {
            if (m_window != nullptr)
                ((AppWindow*)m_window)->m_screen.update();
        }

        /**
         * Updates the strut.
         */
        void updateStrut()
        {
            if (Configuration::is_activateStrut()) {
                if (m_window != nullptr)
                    ((AppWindow*)m_window)->m_screen.struts->update();
            }
        }

        /**
         * removes the current strut
         */
        void removeStrut() 
        {
            if (Configuration::is_activateStrut()) {
                if (m_window != nullptr)
                    ((AppWindow*)m_window)->m_screen.struts->remove();
            }
        }
    }
}

