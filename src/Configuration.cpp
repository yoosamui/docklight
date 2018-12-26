/* 
 * File:   Configuration.cpp
 * Author: yoo
 * 
 * Created on November 4, 2018, 1:32 PM
 */

#include "Configuration.h"
#include "DockWindowHandler.h"

namespace Configuration
{

    panel_locationType get_dockWindowLocation() {
        return panel_locationType::TOP;
    }
    bool is_panelMode()
    {
        return false;
    }
    bool is_activateStrut() {
        if(is_panelMode() && !is_autoHide()){
            return false;
        }
        return false;
    }

    bool is_autoHide() {
        return false;
    }

    unsigned int get_dockWindowSize() {
        return DOCK_WINDOW_HEIGHT;
    }

   
     
//    unsigned int get_topMargin() {
//        return 4;
//    }

//    unsigned int get_bottomMargin() {
//        return 6;
//    }

    unsigned int get_separatorMargin() {
        return 6;
    }

    unsigned int get_CellHeight()
    {
      return Configuration::get_dockWindowSize() - (CELL_BOTTOM_MARGIN + CELL_TOP_MARGIN);
    }
    
    unsigned int get_CellWidth()
    {
      return Configuration::get_dockWindowSize() - CELL_WIDTH_MARGIN;
         
    }
}


