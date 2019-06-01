//*****************************************************************
//
//  Copyright © 2018 Juan R. González
//  j-gonzalez@email.de
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//*****************************************************************


#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#define DEF_DATADIRNAME         "data/"
#define DEF_ATTACHMENTDIR       "data/attachments"
#define DEF_IMAGESDIR           "data/images"
#define DEF_ICONNAME            "data/images/docklight.home.ico"
#define DEF_SEPARATOR           "data/images/separator.png"
#define DEF_SEISSIONICONNAME    "data/images/docklight-session.png"
#define DEF_LOGONAME            "data/images/docklight.logo.png"
#define DEF_INITNAME            "data/docklight.ini"
#define DEF_LAUCHERTEMPLATE     "docklight.template.desktop"
#define DEF_LAUCHERTEMPLATEPATH "data/docklight.template.desktop"
#define DEF_LAUCHERSCRIPT       "docklight.launcher.sh"
#define DEF_LAUCHERSCRIPTPATH   "data/docklight.launcher.sh"
#define DEF_CONFIG_FILE         "data/docklight.config.xml"
#define DEF_MAXSESIONIMAGES     12
#define DEF_LOCALEPATH          "/usr/share/locale"
#define DEF_LOCALEPACKPATH      "/usr/share/locale-langpack"
#define DEF_LAUNCHER_DIR        "/usr/share/applications/"
#define DOCKLIGHT_INSTANCENAME  "docklight"
#define DOCKLIGHTLAUNCHER       "DockLight Launcher"
#define DOCKLIGHT_INSTANCENAME "docklight"

#define NULLPB (Glib::RefPtr<Gdk::Pixbuf>)NULL


#define DOCK_WINDOW_HEIGHT 46

#define CELL_TOP_MARGIN 5                // Cell Top Position
#define CELL_BOTTOM_MARGIN 5

#define CELL_MARGIN 8

#define CELL_WIDTH_MARGIN  8


#define ICON_CELL_WIDTH_MARGIN 12
#define ICON_CELL_TOP_MARGIN  2
#define ICON_CELL_LEFT_MARGIN  2

//#define ICON_CELL_WIDTH_MARGIN  8







////////////////////////////////////////
//#define DEF_ICONMAXSIZE 64
//#define DEF_CELLHIGHT  58                    // Cell Height
//#define DEF_CELLWIDTH 56 //-3//58                    // Cell Width

#define DEF_CELLTOPMARGIN 6                // Cell Top Position
#define DEF_CELLBOTTOMMARGIN 14

//#define DEF_ICONSIZE DEF_CELLHIGHT -14      // Icon Size Width and height
#define DEF_MINCONSIZE 20                   // Minimun Icon Size allowed
#define DEF_ICONTOPMARGIN 10                // Icon top Margin from panel

// Icon top Margin from the cell
#define DEF_ICONTOPCELLMARGIN DEF_ICONTOPMARGIN -DEF_CELLTOPMARGIN

#define DEF_PANELHIGHT DEF_CELLHIGHT + 9
#define DEF_PANELHIGHTHIDE  4



#define DEF_PREVIEW_WIDTH 220               // the preview popup window width
#define DEF_PREVIEW_HEIGHT 180              // the preview popup window height

typedef enum Horizontal_alignment_t
{
    LEFT_TOP,
    RIGHT_BOTTOM,
    CENTER
} Horizontal_alignment_type;

typedef enum panel_location_t {
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
} panel_locationType;

typedef enum StrutsPosition_t {
    Left = 0,
    Right = 1,
    Top = 2,
    Bottom = 3,
    LeftStart = 4,
    LeftEnd = 5,
    RightStart = 6,
    RightEnd = 7,
    TopStart = 8,
    TopEnd = 9,
    BottomStart = 10,
    BottomEnd = 11
} strutsPosition;

typedef enum Window_action_t {
    OPEN,
    CLOSE
} Window_action;

namespace Configuration {



    panel_locationType get_dockWindowLocation();
    Horizontal_alignment_type get_HorizontalAlignment();
    unsigned int get_dockWindowSize();
    unsigned int get_topMargin();
    unsigned int get_bottomMargin();
    unsigned int get_separatorMargin();
    unsigned int get_inmutableSeparatorMargin();
    unsigned int get_itemSize();
    bool is_autoHide();
    void set_autoHide(bool value);
    bool is_activateStrut();
    bool is_panelMode();
    bool is_allowDraw();
    void set_allowDraw(bool value);
    int get_CellHeight();
    int get_CellWidth();
    unsigned int get_WindowDockMonitorMargin_Top();
    unsigned int get_WindowDockMonitorMargin_Right();
    unsigned int get_WindowDockMonitorMargin_Left();
    unsigned int get_WindowDockMonitorMargin_Bottom();
    void set_dockWindowLocation(panel_locationType location);
    unsigned int get_dockWindowSize();
    void set_previewSize(int size);
    unsigned int get_previewSize();
    void set_dockWindowSize(int size);
    void Load();



    namespace Style
    {
        struct Color
        {
            Color(){}
            Color(const double red, const double green, const double blue, const double alpha){

                this->red = red;
                this->green = green;
                this->blue = blue;
                this->alpha = alpha;
            }

            double red = 0.0;
            double green = 0.0;
            double blue = 0.0;
            double alpha = 1.0;
        };

        class ColorWindow
        {
            public:
                ColorWindow(){};
                ColorWindow(const Color fill, const Color stroke, const double lineWith, const double ratio, const int mask)
                {
                    m_Fill = fill;
                    m_Stroke = stroke;
                    m_LineWidth = lineWith;
                    m_Ratio = ratio;
                    m_Mask = mask;
                }

                Color Fill() const  { return m_Fill; }
                Color Stroke() const  { return m_Stroke; }
                double LineWidth() const  { return m_LineWidth;}
                double Ratio() const  {return m_Ratio; }
                int Mask() const  {return m_Mask; }
            private:
                // defaults
                Color m_Fill = Color(0.0, 0.50, 0.66, 1.8);
                Color m_Stroke = Color(0, 0, 0 ,0);
                double m_LineWidth = 1.0;
                double m_Ratio = 6.0;
                int m_Mask = 0;
        };

        class Theme
        {
            public:
                Theme(){};
                ColorWindow& Panel() const { return *m_Panel; }
                ColorWindow& PanelTitle() const { return *m_PanelTitle; }
                ColorWindow& PanelTitleText() const { return *m_PanelTitleText; }
                ColorWindow& PanelCell() const { return *m_PanelCell; }
                ColorWindow& Selector() const { return *m_Selector; }
                ColorWindow& Preview() const { return *m_Preview; }
                ColorWindow& PreviewCell() const { return *m_PreviewCell; }
                ColorWindow& PreviewTitleText() const { return *m_PreviewTitleText; }
                ColorWindow& PreviewClose() const { return *m_PreviewClose; }
                ColorWindow& Separator() const { return *m_Separator; }

                void set_Panel(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_Panel != nullptr){
                        delete m_Panel;
                        m_Panel = nullptr;
                   }

                    m_Panel = cw;
                }

                void set_PanelTitle(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_PanelTitle != nullptr){
                        delete m_PanelTitle;
                        m_PanelTitle = nullptr;
                   }

                    m_PanelTitle = cw;
                }

                void set_PanelTitleText(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_PanelTitleText != nullptr){
                        delete m_PanelTitleText;
                        m_PanelTitleText = nullptr;
                   }

                    m_PanelTitleText = cw;
                }

                void set_PanelCell(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_PanelCell != nullptr){
                        delete m_PanelCell;
                        m_PanelCell = nullptr;
                   }

                    m_PanelCell = cw;
                }

                void set_Selector(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_Selector != nullptr){
                        delete m_Selector;
                        m_Selector = nullptr;
                   }

                    m_Selector = cw;
                }

                void set_Preview(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_Preview != nullptr){
                        delete m_Preview;
                        m_Preview = nullptr;
                   }

                    m_Preview = cw;
                }

                void set_PreviewCell(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_PreviewCell != nullptr){
                        delete m_PreviewCell;
                       m_PreviewCell = nullptr;
                   }

                    m_PreviewCell = cw;
                }

                void set_PreviewTitleText(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_PreviewTitleText != nullptr){
                        delete m_PreviewTitleText;
                       m_PreviewTitleText = nullptr;
                   }

                    m_PreviewTitleText = cw;
                }

                void set_PreviewClose(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_PreviewClose != nullptr){
                        delete m_PreviewClose;
                       m_PreviewClose = nullptr;
                   }

                    m_PreviewClose = cw;
                }

                void set_Separator(ColorWindow* cw) {

                   if (cw == nullptr){
                        return;
                   }

                   if(m_Separator != nullptr){
                        delete m_Separator;
                       m_Separator = nullptr;
                   }

                    m_Separator = cw;
                }
            private:
                ColorWindow* m_Panel = new ColorWindow();
                ColorWindow* m_PanelTitle = new ColorWindow();
                ColorWindow* m_PanelTitleText = new ColorWindow();
                ColorWindow* m_PanelCell = new ColorWindow();
                ColorWindow* m_Selector = new ColorWindow();
                ColorWindow* m_Preview = new ColorWindow();
                ColorWindow* m_PreviewCell = new ColorWindow();
                ColorWindow* m_PreviewTitleText = new ColorWindow();
                ColorWindow* m_PreviewClose = new ColorWindow();
                ColorWindow* m_Separator = new ColorWindow();
        };
    }



    Style::Theme get_Theme();

}

#endif	/* CONFIGURATION_H */


