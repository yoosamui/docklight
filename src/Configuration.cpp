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
    using namespace Style;

    Theme m_theme;
    bool m_autohide = false;
    bool m_allowDraw = true;
    bool m_panelmode = false;
    guint m_separatorMargin = 6;

    panel_locationType m_location = panel_locationType::BOTTOM;
    Horizontal_alignment_type m_HorizontalAlignment =  Horizontal_alignment_type::CENTER;

    void getColorFromString(const std::string& s, Color& fill, Color& stroke, double& lineWidth, double& ratio, int& mask) {

        if (!Utilities::isNumeric(s)) {
            g_critical("The color string is not numeric: [%s]\n", s.c_str());
        }

        std::string currentLocale = setlocale(LC_NUMERIC,NULL);
        setlocale(LC_NUMERIC,"C");

        const int MAXBUFF = 50;
        int maxlength = s.size();
        std::string token = "";
        double values[MAXBUFF] = {0};
        int index = 0;
        for (int i = 0; i < maxlength; i++) {

            if (index < MAXBUFF) {
                char c = s[i];
                if (c != ',' && c != ' ') {
                    token += c;
                }

                if ((c == ',') || i + 1 == maxlength) {

                    double value = 0.0;
                    try {
                        value = std::stod(token);
                        values[ index ] = value;
                        //g_print(" %d........:%f (%s)\n", index, value,token.c_str());

                    } catch (std::invalid_argument) {
                        g_critical("getColorFromString: can't convert the token: %s\n", s.c_str());
                    }


                    token = "";
                    index++;
                }
            }
        }

        fill.red = values[0];
        fill.green = values[1];
        fill.blue = values[2];
        fill.alpha = values[3];

        stroke.red = values[4];
        stroke.green = values[5];
        stroke.blue = values[6];
        stroke.alpha = values[7];

        lineWidth = values[8];
        ratio = values[9];
        mask = (int)values[10];

        setlocale(LC_NUMERIC,currentLocale.c_str());
    }

    void set_DefaultStyle()
    {
        m_theme.set_Panel(new ColorWindow());
        m_theme.set_PanelCell(new ColorWindow(Color(0,0.50,0.66,1), Color(1,1,1,1), 1.5, 3, 0));
        m_theme.set_Selector(new ColorWindow(Color(255,255,255,0.5), Color(1,1,1,1), 1.5, 3, 0));
        m_theme.set_PanelTitle(new ColorWindow(Color(0, 0, 0, 1 ), Color(0,0,0,1), 1, 6, 0));
        m_theme.set_PanelTitleText(new ColorWindow(Color(), Color(1,1,1,1), 1, 0, 0));
    }

    // https://shilohjames.wordpress.com/2014/04/27/tinyxml2-tutorial/
    void Load()
    {
        set_DefaultStyle();

        XMLDocument doc;
        std::string configFile(Utilities::getExecPath(DEF_CONFIG_FILE));
        g_print("Config File: %s\n", configFile.c_str());

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

        // Style Themes
        int useStyle = 0;

        element = root->FirstChildElement("UseStyle");
        XMLCheckResult(element->QueryIntText(&useStyle));

        g_print(".START LOAD STYLE %d\n", useStyle);
        element = root->FirstChildElement("Styles");
        if (element == nullptr){
            g_critical("Configuration Load : Styles ERROR file %s\n", configFile.c_str());
            return;
        }

       // m_theme.
        int id = 0;
        XMLElement* listElement = element->FirstChildElement("Style");
        while (listElement != nullptr){
            const char* name = listElement->GetText();
            const char* panel = listElement->Attribute("panel");
            const char* cell = listElement->Attribute("panelCell");
            const char* panelTitle = listElement->Attribute("panelTitle");
            const char* selector = listElement->Attribute("selector");

            XMLCheckResult(listElement->QueryIntText(&id));
            if(id == useStyle ){
                Color fill;
                Color stroke;
                double lineWidth;
                double ratio;
                int mask;

                if (panel != nullptr){
                    getColorFromString(panel, fill, stroke, lineWidth, ratio,  mask);
                    m_theme.set_Panel(new ColorWindow(fill,stroke,lineWidth,ratio,mask));
                g_print("Style %d %s PANEL  Fill:%f %f %f %f Stroke %f %f %f %f  MASK %d\n",id, panel, fill.red, fill.green, fill.blue, fill.alpha, stroke.red, stroke.green, stroke.blue, stroke.alpha, mask);
                }

               if (selector != nullptr){
                    getColorFromString(selector, fill, stroke, lineWidth, ratio,  mask);
                    m_theme.set_Selector(new ColorWindow(fill,stroke,lineWidth,ratio,mask));
                }

                if (cell != nullptr){
                    getColorFromString(cell, fill, stroke, lineWidth, ratio,  mask);
                    m_theme.set_PanelCell(new ColorWindow(fill,stroke,lineWidth,ratio,mask));
                g_print("Style %d %s CELL   Fill:%f %f %f %f Stroke %f %f %f %f \n",id, cell, fill.red, fill.green, fill.blue, fill.alpha, stroke.red, stroke.green, stroke.blue, stroke.alpha);

                }

                if (panelTitle != nullptr){
                    getColorFromString(panelTitle, fill, stroke, lineWidth, ratio,  mask);
                    m_theme.set_PanelTitle(new ColorWindow(fill,stroke,lineWidth,ratio,mask));

                }


                break;
            }

            listElement =listElement->NextSiblingElement("Style");
        }
    }

    Style::Theme get_Theme(){
        return m_theme;
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

    namespace Style
    {

        /**
         * Constructs the default theme
         */
        //Theme::Theme() {

            // Default theme
             //this->m_Panel = new ColorWindow();
             //this->m_PanelTitle = new ColorWindow(Color(0, 0, 0, 1 ), Color(0,0,0,1), 1, 6, 0);
             //this->m_PanelTitleText = new ColorWindow(Color(), Color(1,1,1,1), 1, 0, 0);
             //this->m_PanelCell = new ColorWindow(Color(0,0.50,0.66,1), Color(1,1,1,1), 1.5, 3, 0);



//            cr->set_source_rgba(0.00, 0.50, 0.66, 1);



/*
               Color foreColor(1.0, 1.0, 1.0, 1.0);

               Color backSelector(1.9, 1.0, 1.0, 0.3);
               Color foreSelector(1.1, 1.1, 1.0, 1.0);

               this->m_window = new ColorWindow();
               this->m_panel = new ColorWindow(true, backColor, foreColor, 0.7, 4.0);
               this->m_panelSelector = new ColorWindow(true, backSelector, foreSelector, 1.5, 4.0);
               this->m_panelTitle = new ColorWindow(true, backColor, foreColor, 0.0, 4.0);
               this->m_panelTitleText = new ColorWindow(true, Color(), Color(1.0, 1.0, 1.0, 1.0), 0.0, 0.0);
               this->m_preview = new ColorWindow(true, backColor, foreColor, 0.7, 4.0);
               this->m_previewTitle = new ColorWindow(true, Color(0.0, 0.0, 1.0, 0.0), Color(1.0, 1.0, 1.0, 0.0), 1.0, 0.0);
               this->m_previewTitleText = new ColorWindow(true, Color(), Color(), 0.0, 0.0);
               this->m_previewSelector = new ColorWindow(true, backSelector, foreSelector, 2.0, 4.0);
               this->m_previewSelectorClose = new ColorWindow(true, Color(0.0, 0.50, 0.0, 1.0), foreColor, 4.0, 4.0);
               */
       // }
    }
}


