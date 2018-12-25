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
        if(is_panelMode() && !is_autoHide()){
            return true;
        }
        return false;
    }

    bool is_autoHide() {
        return false;
    }

    unsigned int get_dockWindowSize() {
        return 64;
    }

    
    unsigned int get_topMargin() {
        return 4;
    }

    unsigned int get_bottomMargin() {
        return 6;
    }

    unsigned int get_separatorMargin() {
        return 6;
    }

    unsigned int get_itemSize() {
        return DEF_CELLWIDTH; // DockWindowHandler::DockWindow::get_size() - (  get_separatorMargin() + get_bottomMargin());
    }
}


