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
    char titlename[DEF_FIELD_MAX] = {'\0'};
    guint8 pixbuff[ DEF_PIXBUFFER_MAX] = {0};
    char appname[DEF_FIELD_MAX] = {'\0'};
    char instancename[DEF_FIELD_MAX] = {'\0'};
    char realgroupname[DEF_FIELD_MAX] = {'\0'};
    char themeiconname[DEF_FIELD_MAX] = {'\0'};

    guint dockitemtype = 0;
    int index = -1;
    guint width = 0;
    guint height = 0;
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
    bool DettachItem(const int index);
    bool SwapItems(guint sourceIdx, guint targetIdx);
    void MoveItem(const int next_position);
    void Reindex();
private:
    static void Update(WnckWindow* window, Window_action actiontype);
    static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);
    static void on_theme_changed(GtkSettings *settings, GParamSpec *pspec, GtkIconTheme *icon_theme);
    static void setIconByTheme(DockItem *item);
    static bool get_IsLimitsReached();
};

#endif	/* APPUPDATER_H */

