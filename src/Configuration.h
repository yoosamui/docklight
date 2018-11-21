/* 
 * File:   Configuration.h
 * Author: yoo
 *
 * Created on November 4, 2018, 1:32 PM
 */

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#define DEF_FRAMERATE 1000/30
#define DOCKLIGHT_INSTANCENAME  "docklight"
#define DEF_ICONMAXSIZE 64

#define DEF_DATADIRNAME         "data/" 
#define DEF_ATTACHMENTDIR       "data/attachments" 
#define DEF_IMAGESDIR           "data/images" 
#define DEF_ICONNAME            "data/images/docklight.home.ico"
#define DEF_SEISSIONICONNAME    "data/images/docklight-session.png"
#define DEF_LOGONAME            "data/images/docklight.logo.png"
#define DEF_INITNAME            "data/docklight.ini"
#define DEF_LAUCHERTEMPLATE     "docklight.template.desktop"
#define DEF_LAUCHERTEMPLATEPATH "data/docklight.template.desktop"
#define DEF_LAUCHERSCRIPT       "docklight.launcher.sh"
#define DEF_LAUCHERSCRIPTPATH   "data/docklight.launcher.sh"
#define DEF_MAXSESIONIMAGES     12
#define DEF_LOCALEPATH          "/usr/share/locale"
#define DEF_LOCALEPACKPATH      "/usr/share/locale-langpack"
#define DEF_LAUNCHER_DIR        "/usr/share/applications/"
#define DOCKLIGHT_INSTANCENAME  "docklight"
#define DOCKLIGHTLAUNCHER       "DockLight Launcher"
#define DOCKLIGHT_INSTANCENAME "docklight"
#define NULLPB (Glib::RefPtr<Gdk::Pixbuf>)NULL


#define DEF_ICONMAXSIZE 64
#define DEF_CELLHIGHT  56                    // Cell Height
#define DEF_CELLWIDTH DEF_CELLHIGHT -3//58                    // Cell Width
#define DEF_CELLTOPMARGIN 6                 // Cell Top Position
#define DEF_ICONSIZE DEF_CELLHIGHT -14      // Icon Size Width and height
#define DEF_MINCONSIZE 20                   // Minimun Icon Size allowed
#define DEF_ICONTOPMARGIN 10                // Icon top Margin from panel

// Icon top Margin from the cell
#define DEF_ICONTOPCELLMARGIN DEF_ICONTOPMARGIN -DEF_CELLTOPMARGIN                

#define DEF_PANELHIGHT DEF_CELLHIGHT + 9
#define DEF_PANELHIGHTHIDE  4          



#define DEF_PREVIEW_WIDTH 220               // the preview popup window width
#define DEF_PREVIEW_HEIGHT 200              // the preview popup window height

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


namespace Configuration {



    panel_locationType get_dockWindowLocation();
    unsigned int get_dockWindowSize();
    unsigned int get_topMargin();
    unsigned int get_bottomMargin();
    unsigned int get_separatorMargin();
    unsigned int get_itemSize();
    bool is_autoHide();
     bool is_activateStrut();
}

#endif	/* CONFIGURATION_H */

