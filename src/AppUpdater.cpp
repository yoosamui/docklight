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


#include "AppUpdater.h"
#include "DockItem.h"
#include "DockWindow.h"
#include "Launcher.h"
#include "DockItemPositions.h"
#include <fstream>
#include <iostream>

// static members
std::vector<DockItem*> AppUpdater::m_dockitems;
AppUpdater::AppUpdater()
{
    // load attachments
    this->Load();

    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();

    g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(AppUpdater::on_window_opened), NULL);
    g_signal_connect(wnckscreen, "window_closed", G_CALLBACK(AppUpdater::on_window_closed), NULL);


    // signal for icon themes changes
    GdkScreen *screen = gdk_screen_get_default();
    auto icon_theme = g_object_get_data(G_OBJECT(screen), "gtk-icon-theme");
    auto settings = gtk_settings_get_for_screen(screen);
    g_signal_connect(settings, "notify::gtk-icon-theme-name", G_CALLBACK(AppUpdater::on_theme_changed), icon_theme);


}

/**
 * Destructor.
 */
AppUpdater::~AppUpdater()
{
    for (int i = m_dockitems.size() - 1; i >= 0; i--) {
        delete(m_dockitems[i]);
        m_dockitems[i] = NULL;
        m_dockitems.erase(m_dockitems.begin() + i);
    }

    g_print("AppUpdater destroy.\n");
}

/**
 * Load Attachments
 */
void AppUpdater::Load()
{
    std::string filePath = this->getFilePath();
    size_t result;
    FILE* f;
    struct attachments_data st;

    f = fopen(filePath.c_str(), "rb");
    if (!f) {
        g_critical("Load: can't open file.");
        return;
    }
    int index = 1;
    while (1) {
        result = fread(&st, sizeof (st), 1, f);

        if (feof(f) != 0)
            break;

        if (result == 0)
            g_critical("Load Error reading file fread\n");

        DockItem* dockItem = new DockItem(st.width, st.height);
        dockItem->m_isAttached = true;
        dockItem->m_appname = st.appname;
        dockItem->m_titlename = st.titlename;
        dockItem->m_realgroupname = st.realgroupname;
        dockItem->m_instancename = st.instancename;
        dockItem->m_theme_iconname = st.themeiconname;
        dockItem->m_dockitemtype = (DockItemType)st.dockitemtype;
        dockItem->m_isDirty = true;
        dockItem->m_image = NULLPB;
        dockItem->m_index  = index;

        /*
           std::string theme_iconname ="";
           dockItem->m_image = IconLoader::GetIconByAppName(
           dockItem->m_instancename.c_str(),theme_iconname);
           */

        if (st.pixbuff[0] != 0) {
            try {
                auto loader = Gdk::PixbufLoader::create();
                loader->write(st.pixbuff, sizeof (st.pixbuff));
                dockItem->m_image = loader->get_pixbuf();
                loader->close();
            }
            catch (Gdk::PixbufError pe) {
                g_critical("AppUpdater Load: Gdk::PixbufError\n");
            }
            catch (Glib::FileError fe) {
                g_critical("AppUpdater Load: Glib::FileError\n");
            }

        }


        setIconByTheme(dockItem);
        m_dockitems.push_back(std::move(dockItem));

        index++;

        if (get_IsLimitsReached()){
            g_warning("Load: Decrement limimit reached.\n");
            break;
        }
    }

    fclose(f);
}

/**
 *
 * Save Attachments
 */
void AppUpdater::Save()
{
    std::string filePath = this->getFilePath();

    FILE* f;
    f = fopen(filePath.c_str(), "wb");
    if (!f) {
        g_critical("SessionWindow::save: can't create file.");
        return;
    }

    gchar* iconBuffer;
    gsize buffer_size;
    guint idx;
    struct attachments_data st;

    for (idx = 1; idx < this->m_dockitems.size(); idx++) {

        DockItem* item = this->m_dockitems[idx];

        if (!item->m_isAttached) {
            continue;
        }


        if (item->m_image != NULLPB) {
            try {
                item->m_image->save_to_buffer(iconBuffer, buffer_size);
                memcpy(st.pixbuff, iconBuffer, buffer_size);
                delete [] (gchar*)iconBuffer;
            }
            catch (Gdk::PixbufError pe) {
                g_critical("Attachments::save: Gdk::PixbufError\n");
            }
            catch (Glib::FileError fe) {
                g_critical("Attachments::save: Glib::FileError\n");
            }

            if (buffer_size > DEF_PIXBUFFER_MAX) {
                g_critical("Attachments::save: pixbuf  size out of range.");
                continue;
            }
        }

        strncpy(st.appname, item->m_appname.c_str(), DEF_FIELD_MAX);
        strncpy(st.titlename, item->m_titlename.c_str(), DEF_FIELD_MAX);
        strncpy(st.realgroupname, item->m_realgroupname.c_str(), DEF_FIELD_MAX);
        strncpy(st.instancename, item->m_instancename.c_str(), DEF_FIELD_MAX);
        strncpy(st.themeiconname, item->m_theme_iconname.c_str(), DEF_FIELD_MAX);

        st.width = item->get_InmutableWidth();
        st.height = item->get_InmutableHeight();

        st.dockitemtype = item->m_dockitemtype;

        size_t result = fwrite(&st, sizeof (st), 1, f);
        if (result == 0)
            g_critical("Attachments::save:: Error writing file> fwrite\n");
    }

    fclose(f);
}
/**
 * gets the next available index.
 */
//guint AppUpdater::get_NextIndex()
//{
    //guint index = 0;

    //for (auto item : m_dockitems){
        //if (index > item->m_index){
            //index = item->m_index;
        //}
    //}
    //g_print("NextIndex %d\n",index);
    //return index;
//}
/**
 * Attacht the item by the given index.
 */
bool AppUpdater::AttachItem(const int index)
{
    if (index < 0 || index > this->m_dockitems.size()){
        return false;
    }

    DockItem* item = m_dockitems[index];
    if (item->m_isAttached){
        return false;
    }

    item->m_index = index;
    item->m_isAttached = true;

    this->Save();

    return true;
}


bool AppUpdater::DettachItem(const int index)
{
    if (index < 0 || index > this->m_dockitems.size()){
        return false;
    }

    DockItem* item =  this->m_dockitems[index];
    item->m_isAttached = false;

    if (item->m_items.size() > 0){
        this->Save();
        return true;
    }

    return this->RemoveItem(index);
}

/**
 * Remove an Item from vector by the given index.
 */
bool AppUpdater::RemoveItem(const int index)
{
    if (index < 0 || index > this->m_dockitems.size()){
        return false;
    }

    delete this->m_dockitems[index];
    this->m_dockitems[index] = NULL;

    // Deletes the element by index;
    this->m_dockitems.erase(this->m_dockitems.begin() + index);
    this->Reindex();

    DockWindow::update();

    this->Save();
    return true;
}


void AppUpdater::MoveItem(const int next_position)
{

    static guint position = 0;

    if (next_position < 0 || next_position > this->m_dockitems.size()){
        return;
    }

    if(position == 0 || next_position == 0){
        position = next_position;
        return;
    }

    if(position == next_position ){
        return;
    }

   iter_swap(this->m_dockitems.begin() + position, this->m_dockitems.begin() + next_position);
   position = next_position;

   DockPanel::update();
}

std::string AppUpdater::getFilePath()
{
    std::string path = Utilities::getExecPath();
    char buff[PATH_MAX];
    sprintf(buff, "%s/%s", path.c_str(), DEF_ATTACHMENTDIR);
    Utilities::CreateDirectoryIfNotExitst(buff);
    sprintf(buff, "%s/%s/attachments.conf", path.c_str(), DEF_ATTACHMENTDIR);

    return buff;
}

/**
 * Emitted when a new Wnck.Window is opened on screen.
 * @param screen
 * @param window
 * @param data
 */
void AppUpdater::on_window_opened(WnckScreen *screen, WnckWindow* window, gpointer data)
{

    Update(window, Window_action::OPEN);
}

/**
 * Emitted when a Wnck.Window is closed on screen.
 * @param screen
 * @param window
 * @param data
 */
void AppUpdater::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    // updateSessioWindow(window);
    Update(window, Window_action::CLOSE);
    DockWindow::update();
}

void AppUpdater::Update(WnckWindow* window, Window_action actiontype)
{
    WnckWindowType wt = wnck_window_get_window_type(window);

    if (wt == WNCK_WINDOW_DESKTOP ||
            wt == WNCK_WINDOW_DOCK ||
            wt == WNCK_WINDOW_MENU ||
            wt == WNCK_WINDOW_SPLASHSCREEN) {
        return;
    }

    if (actiontype == Window_action::OPEN) {
        std::string the_appname;
        std::string the_instancename;
        std::string the_groupname;
        std::string the_titlename;

        if (Launcher::getAppNameByWindow(window,
                    the_appname,
                    the_instancename,
                    the_groupname,
                    the_titlename) == FALSE) {
            return;
        }

        //DEBUG
        g_print("...appnames: %s, %s, %s title:%s\n",
                the_appname.c_str(),
                the_instancename.c_str(),
                the_groupname.c_str(),
                the_titlename.c_str());

        /*

           if (m_currentsessionItem != nullptr && m_sessiondata.size() > 0) {
           for (int i = m_sessiondata.size() - 1; i >= 0; i--) {
           std::string appname(m_sessiondata[i].appname);
           if (appname == the_groupname) {
           if (attachToSessiongrp(window)) {
           m_sessiondata.erase(m_sessiondata.begin() + i);
           break;
           }
           }
           }
           }
           */


        // handle DockItems groups
        for (auto item:AppUpdater::m_dockitems) {
            if (the_groupname == item->m_realgroupname) {
                DockItem* childItem = new DockItem();
                childItem->m_realgroupname = the_groupname;
                childItem->m_appname = the_appname;
                childItem->m_titlename = the_titlename;
                childItem->m_instancename = the_instancename;
                childItem->m_isDirty = false;
                childItem->m_window = window;
                childItem->m_xid = wnck_window_get_xid(window);
                childItem->m_image = NULLPB;

                if (item->m_isAttached && item->m_isDirty) {
                    item->m_window = window;
                    item->m_xid = childItem->m_xid;
                    item->m_isDirty = false;

                    for (auto itemtorelease:item->m_items)
                        delete(itemtorelease);

                    item->m_items.clear();
                }

                item->m_items.push_back(childItem);
                return;
            }
        }

        if (get_IsLimitsReached()){
            return;
        }

        Glib::RefPtr<Gdk::Pixbuf> appIcon = NULLPB;

        std::string theme_iconname = "";
        appIcon = IconLoader::GetWindowIcon(window, theme_iconname);
        appIcon = appIcon->scale_simple(DEF_ICONMAXSIZE,
                DEF_ICONMAXSIZE, Gdk::INTERP_BILINEAR);

        // Create a new Item
        DockItem* dockItem = new DockItem();
        dockItem->m_appname = the_appname;
        dockItem->m_titlename = the_titlename;
        dockItem->m_realgroupname = the_groupname;
        dockItem->m_instancename = the_instancename;
        dockItem->m_window = window;
        dockItem->m_xid = wnck_window_get_xid(window);
        dockItem->m_image = appIcon;
        dockItem->m_theme_iconname = theme_iconname;
        dockItem->m_index = m_dockitems.size();

        // Create a child items
        DockItem* childItem = new DockItem();
        childItem->m_appname = dockItem->m_appname;
        childItem->m_titlename = the_titlename;
        childItem->m_realgroupname = dockItem->m_realgroupname;
        childItem->m_instancename = dockItem->m_instancename;
        childItem->m_window = dockItem->m_window;
        childItem->m_xid = dockItem->m_xid;
        childItem->m_image = NULLPB;

        dockItem->m_items.push_back(childItem);
        m_dockitems.push_back(std::move(dockItem));

        DockWindow::update();

    }
    else {
        // find the item to remove;
        int xid = wnck_window_get_xid(window);
        int i = 1;
        bool found = false;
        for (; i < (int)m_dockitems.size(); i++) {
            for (auto ci:m_dockitems.at(i)->m_items) {
                if (ci->m_xid == xid) {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (xid > 0 && found) {
            auto item = m_dockitems.at(i);
            if (item->m_items.size() == 1) {
                // if is attached delete the child item.
                if (item->m_isAttached) {
                    item->m_isDirty = true;

                    delete( item->m_items.at(0));
                    item->m_items.erase(item->m_items.begin());

                    return;
                }

                // remove this item
                delete( item->m_items.at(0));
                m_dockitems.erase(m_dockitems.begin() + i);

                // if is not attached then it is at the end on the list.
                // we need to reset the index.
                //  m_currentMoveIndex = -1;

                return;
            }
            else {
                // search for the xid and remove the item
                int idx = 0;
                for (auto c:item->m_items) {
                    if (c->m_xid == xid) {
                        delete(c);
                        item->m_items.erase(item->m_items.begin() + idx);
                        return;
                    }
                    idx++;
                }
            }
        }
    }


}

/**
 *  We check the limits here. We avoid that incomming items are out of range.
 *  If the items width/height are to small this function will return true.
 *  Otherwise false.
 */
bool AppUpdater::get_IsLimitsReached()
{

    if (m_dockitems.size() == 0 ){
        return false;
    }

    guint decrement = 0;
    if (DockWindow::is_Horizontal()){

        decrement = DockItemPositions::get_ResizeWidthDecrement();
        if (decrement > 0 && m_dockitems[0]->get_InmutableWidth() - decrement  <= 26){
            return true;
        }
    }
    else {

        decrement = DockItemPositions::get_ResizeHeightDecrement();
        if (decrement > 0 && m_dockitems[0]->get_InmutableHeight() - decrement  <= 26){
            return true;
        }
    }

    return false;
}

void AppUpdater::setIconByTheme(DockItem *item)
{
    if (!item->m_image || item->m_theme_iconname == "") {
        return;
    }

    const char* appname = item->m_instancename.c_str();
    auto image = IconLoader::GetIconByAppName(appname, item->m_theme_iconname);
    if (image) {
        item->m_image = image;
    }
}


void AppUpdater::Reindex()
{
    guint idx = 0;

    for (auto item : this->m_dockitems){
        item->m_index = idx;

        idx++;
    }
}


bool AppUpdater::SwapItems(guint sourceIdx, guint targetIdx)
{
    if (sourceIdx == targetIdx || sourceIdx < 1 || targetIdx < 1 || sourceIdx > this->m_dockitems.size() || targetIdx > this->m_dockitems.size()){
        return false;
    }

    // save source
    DockItem* tmp = this->m_dockitems[sourceIdx];
    tmp->m_isAttached = true;

    // Deletes the element by index and insert the source element in the target;
    this->m_dockitems.erase(this->m_dockitems.begin() + sourceIdx);
    this->m_dockitems.insert(this->m_dockitems.begin() + targetIdx, tmp);

    this->Reindex();
    this->Save();

}
void AppUpdater::on_theme_changed(GtkSettings *settings, GParamSpec *pspec, GtkIconTheme *icon_theme)
{
    for (DockItem* item:m_dockitems) {
        setIconByTheme(item);
    }
}


