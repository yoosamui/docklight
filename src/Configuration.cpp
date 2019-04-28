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
#include "tinyxml2.h"

using namespace tinyxml2;

#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { g_critical("Error: %i\n", a_eResult); return; }
#endif

namespace Configuration
{
    bool m_autohide = false;
    bool m_allowDraw = true;
    bool m_panelmode = false;
    guint m_separatorMargin = 8;

    panel_locationType m_location = panel_locationType::BOTTOM;
    Horizontal_alignment_type m_HorizontalAlignment =  Horizontal_alignment_type::CENTER;

    // https://shilohjames.wordpress.com/2014/04/27/tinyxml2-tutorial/
    void Load()
    {
        XMLDocument doc;
        std::string configFile(Utilities::getExecPath(DEF_CONFIG_FILE));
        XMLError result = doc.LoadFile(configFile.c_str());
        XMLCheckResult(result);

        XMLNode* root = doc.FirstChild();
        if (root == nullptr){
            g_critical("Configuration Load : XML_ERROR_FILE_READ_ERROR file %s\n", configFile.c_str());
            return;
        }

        XMLElement* element = root->FirstChildElement("AutoHide");
        XMLCheckResult(element->QueryBoolText(&m_autohide));

        element = root->FirstChildElement("PanelMode");
        XMLCheckResult(element->QueryBoolText(&m_panelmode));

        element = root->FirstChildElement("Location");
        const char* location = element->GetText();
        if (location != NULL) {

            if (strcmp(location, "TOP") == 0) {
                m_location =  panel_locationType::TOP;
            }
            else if (strcmp(location, "RIGHT") == 0) {
                m_location =  panel_locationType::RIGHT;
            }
            else if (strcmp(location, "BOTTOM") == 0) {
                m_location =  panel_locationType::BOTTOM;
            }
            else if (strcmp(location, "LEFT") == 0) {
                m_location =  panel_locationType::LEFT;
            }
        }

        // Styles
        element = root->FirstChildElement("Styles");
        if (element == nullptr){
            g_critical("Configuration Load : Styles ERROR file %s\n", configFile.c_str());
            return;
        }


                g_print(".START LIST CONF\n");
        XMLElement* listElement = element->FirstChildElement("Style");
        while (listElement != nullptr){


            const char* name = listElement->GetText();
            if (name != nullptr){
                g_print("............Stylename:%s\n",name);
            }

            const char* panel = listElement->Attribute("panel");
            if (panel != nullptr){
                g_print(".............Panel%s\n",panel);
            }

            const char* cell = listElement->Attribute("cell");
            if (cell != nullptr){
                g_print(".............Cell%s\n",cell);
            }

            const char* selector = listElement->Attribute("selector");
            if (selector != nullptr){
                g_print(".............selector%s\n",selector);
            }


            listElement =listElement->NextSiblingElement("Style");

        }


    }

    Horizontal_alignment_type get_HorizontalAlignment()
    {
        return m_HorizontalAlignment;
    }

    panel_locationType get_dockWindowLocation()
    {

        return m_location;
    }

    void set_dockWindowLocation(panel_locationType location)
    {
        m_location = location;
    }

    bool is_panelMode()
    {
        return m_panelmode; // Explicite set
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


