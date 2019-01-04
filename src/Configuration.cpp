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
        return panel_locationType::BOTTOM;
    }
    bool is_panelMode()
    {
        return false;
    }
    bool is_activateStrut() {
         return true;
        if(is_panelMode() && !is_autoHide()){
            return false;
        }
        return true;
    }

    bool is_autoHide() {
        return false;
    }

    // TODO: REMOVE THIS
    unsigned int get_dockWindowSize() {
        return 50;//DOCK_WINDOW_HEIGHT;
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
      return Configuration::get_dockWindowSize() - (CELL_BOTTOM_MARGIN + CELL_TOP_MARGIN) ;
    }
    
    unsigned int get_CellWidth()
    {
      return get_CellHeight() - 2;//CELL_WIDTH_MARGIN;
         
    }
}


