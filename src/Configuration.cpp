/*
//# The above copyright notice and this permission notice shall be included in
//# all copies or substantial portions of the Software.
 * File:   Configuration.cpp
 * Author: yoo
 *
 * Created on November 4, 2018, 1:32 PM
 */

#include "Configuration.h"
#include "DockWindow.h"

namespace Configuration
{
    bool m_autohide = false;
    bool m_allowDraw = true;
    guint m_separatorMargin = 8;

    panel_locationType m_locatiom = panel_locationType::BOTTOM;
    Horizontal_alignment_type m_HorizontalAlignment =  Horizontal_alignment_type::CENTER;

    Horizontal_alignment_type get_HorizontalAlignment()
    {
        return m_HorizontalAlignment;
    }

    panel_locationType get_dockWindowLocation()
    {
        return m_locatiom;
    }

    void set_dockWindowLocation(panel_locationType location)
    {
        m_locatiom = location;
    }

    bool is_panelMode()
    {
        return true; // Explicite set
    }

    bool is_activateStrut()
    {
        return is_autoHide() == false;
    }

    bool is_autoHide()
    {
        return m_autohide;
    }

    void set_autoHide(bool value)
    {
        m_autohide = value;
    }


    bool is_allowDraw()
    {
        return m_allowDraw;
    }

    void set_allowDraw(bool value)
    {
        m_allowDraw = value;
    }

    unsigned int get_dockWindowSize()
    {
        return 56; //DOCK_WINDOW_AREA;
    }

    unsigned int get_WindowDockMonitorMargin_Top()
    {
        if(is_panelMode() || !is_autoHide()){
            return 0;
        }

        return 27;
    }
    unsigned int get_WindowDockMonitorMargin_Right()
    {
          if(is_panelMode() || !is_autoHide()){
            return 0;
        }

        return 4;
    }

    unsigned int get_WindowDockMonitorMargin_Left()
    {
          if(is_panelMode() || !is_autoHide()){
            return 0;
        }

        return 4;
    }

    unsigned int get_WindowDockMonitorMargin_Bottom()
    {
       if(is_panelMode() || !is_autoHide()){
            return 0;
        }

        return 4;
    }

    unsigned int get_separatorMargin()
    {
        return m_separatorMargin;
    }

    unsigned int get_inmutableSeparatorMargin()
    {
        return m_separatorMargin;
    }


    unsigned int get_CellHeight()
    {
        return Configuration::get_dockWindowSize() - (CELL_BOTTOM_MARGIN + CELL_TOP_MARGIN);

    }

    unsigned int get_CellWidth()
    {
        return get_CellHeight() - 2; //CELL_WIDTH_MARGIN;

    }
}


