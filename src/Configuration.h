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
#define CELL_TOP_MARGIN 8                // Cell Top Position

#define CELL_MARGIN 8

#define CELL_BOTTOM_MARGIN 8
#define CELL_WIDTH_MARGIN  8


#define ICON_CELL_WIDTH_MARGIN 12
#define ICON_CELL_TOP_MARGIN  2
#define ICON_CELL_LEFT_MARGIN  2

//#define ICON_CELL_WIDTH_MARGIN  8







////////////////////////////////////////
#define DEF_ICONMAXSIZE 64
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
#define DEF_PREVIEW_HEIGHT 200              // the preview popup window height

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
    unsigned int get_CellHeight();
    unsigned int get_CellWidth();
    unsigned int get_WindowDockMonitorMargin_Top();
    unsigned int get_WindowDockMonitorMargin_Right();
    unsigned int get_WindowDockMonitorMargin_Left();
    unsigned int get_WindowDockMonitorMargin_Bottom();
    void set_dockWindowLocation(panel_locationType location);
    //void set_SeparatorMargin(unsigned int  margin);
    //
    //
    void Load();

}

#endif	/* CONFIGURATION_H */

