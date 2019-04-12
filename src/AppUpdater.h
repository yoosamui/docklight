/* 
 * File:   AppUpdater.h
 * Author: yoo
 *
 * Created on February 16, 2019, 12:52 PM
 */

#ifndef APPUPDATER_H
#define	APPUPDATER_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm.h> 

#include "Configuration.h"
#include "DockItem.h"
#include "SessionWindow.h"

#define DEF_PIXBUFFER_MAX 1024 << 3
#define DEF_FIELD_MAX  64

struct attachments_data
{
    guint8 pixbuff[ DEF_PIXBUFFER_MAX] = {0};
    char appname[DEF_FIELD_MAX] = {'\0'};
    char titlename[DEF_FIELD_MAX] = {'\0'};
    char instancename[DEF_FIELD_MAX] = {'\0'};
    char realgroupname[DEF_FIELD_MAX] = {'\0'};
    char themeiconname[DEF_FIELD_MAX] = {'\0'};
                                         
    int attachedIndex = -1;
    int dockitemtype = 0;
    int index = -1;
    int width = 0;
    int height = 0;
};

class AppUpdater {
public:
    AppUpdater();
    virtual ~AppUpdater();
    static std::vector<DockItem*> m_dockitems;
    void Load();
    void Save();
    std::string getFilePath();
    bool RemoveItem(const int index);
    bool AttachItem(const int index);
private:
    static void Update(WnckWindow* window, Window_action actiontype);
    static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);
    static void on_theme_changed(GtkSettings *settings, GParamSpec *pspec, GtkIconTheme *icon_theme);
    static void setIconByTheme(DockItem *item);
    static bool get_IsLimitsReached();
};

#endif	/* APPUPDATER_H */

