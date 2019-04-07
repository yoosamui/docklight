/* 
 * File:   AppUpdater.cpp
 * Author: yoo
 * 
 * Created on February 16, 2019, 12:52 PM
 */

#include "AppUpdater.h"
#include "DockWindow.h"
#include "Launcher.h"

#include <fstream>
#include <iostream>

// static members
std::vector<DockItem*> AppUpdater::m_dockitems;

AppUpdater::AppUpdater()
{
    this->Load();

    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();

    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                     G_CALLBACK(AppUpdater::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
                     G_CALLBACK(AppUpdater::on_window_closed), NULL);


    // signal for icon themes changes
    GdkScreen *screen = gdk_screen_get_default();
    auto icon_theme = g_object_get_data(G_OBJECT(screen), "gtk-icon-theme");
    auto settings = gtk_settings_get_for_screen(screen);
    g_signal_connect(settings, "notify::gtk-icon-theme-name",
                     G_CALLBACK(AppUpdater::on_theme_changed), icon_theme);


}

AppUpdater::~AppUpdater(){ }

/**
 * Load Attachments
 */
void AppUpdater::Load()
{
    std::string filePath = this->getFilePath();
    size_t result;
    int index = 1;
    FILE* f;
    f = fopen(filePath.c_str(), "rb");
    if (!f) {
        g_critical("Load: can't open file.");
        return;
    }

    struct attachments_data st;

    while (1) {
        result = fread(&st, sizeof (st), 1, f);

        if (feof(f) != 0)
            break;

        if (result == 0)
            g_critical("Load Error reading file fread\n");

        DockItem* dockItem = new DockItem();
        dockItem->m_isAttached = true;
        dockItem->m_appname = st.appname;
        dockItem->m_titlename = st.titlename;
        dockItem->m_realgroupname = st.realgroupname;
        dockItem->m_instancename = st.instancename;
        dockItem->m_theme_iconname = st.themeiconname;
        dockItem->m_dockitemtype = (DockItemType)st.dockitemtype;
        dockItem->m_width = st.width;
        dockItem->m_height = st.height;
        dockItem->m_index = index;//st.index;
        dockItem->m_attachedIndex = st.index;
        dockItem->m_isDirty = true;


        /*
       std::string theme_iconname ="";
       dockItem->m_image = IconLoader::GetIconByAppName(
               dockItem->m_instancename.c_str(),theme_iconname);
         */

        if (st.pixbuff) {
            try {
                auto loader = Gdk::PixbufLoader::create();
                loader->write(st.pixbuff, sizeof (st.pixbuff));
                dockItem->m_image = loader->get_pixbuf();
                loader->close();
            }
            catch (Gdk::PixbufError pe) {
                g_critical("SessionWindow::init: Gdk::PixbufError\n");
            }
            catch (Glib::FileError fe) {
                g_critical("SessionWindow::init: Glib::FileError\n");
            }

        }

        setIconByTheme(dockItem);
        m_dockitems.push_back(std::move(dockItem));
        index++;
    }

    fclose(f);

}

/**
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
    struct attachments_data st;

    int index = 0;
    for (DockItem *item:m_dockitems) {

        if (index == 0) {
            index++;
            continue;
        }

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

        item->m_index = index;
        st.attachedIndex = item->m_attachedIndex;
        st.dockitemtype = (int)item->m_dockitemtype;
        st.width = item->m_width;
        st.height = item->m_height;

        st.index = (int)item->m_index;

        size_t result = fwrite(&st, sizeof (st), 1, f);
        if (result == 0)
            g_critical("Attachments::save:: Error writing file> fwrite\n");

        // g_print("%d\n",index);
        index++;
    }

    fclose(f);
}

std::string AppUpdater::getFilePath()
{
    std::string path = Utilities::getExecPath();
    char buff[PATH_MAX];
    sprintf(buff, "%s/%s", path.c_str(), DEF_ATTACHMENTDIR);
    Utilities::CreateDirectoryIfNotExitst(buff);
    sprintf(buff, "%s/%s/attachments.dat", path.c_str(), DEF_ATTACHMENTDIR);

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

                  ///////////////////////  DockWindow::update();
                    return;
                }

                // remove this item
                delete( item->m_items.at(0));
                m_dockitems.erase(m_dockitems.begin() + i);

                // if is not attached then it is at the end on the list.
                // we need to reset the index.
                //  m_currentMoveIndex = -1;

               /////////////////////////////// DockWindow::update();
                return;
            }
            else {
                // search for the xid and remove the item
                int idx = 0;
                for (auto c:item->m_items) {
                    if (c->m_xid == xid) {
                        delete(c);
                        item->m_items.erase(item->m_items.begin() + idx);
                       //////////////// DockWindow::update();
                        return;
                    }
                    idx++;
                }
            }
        }
    }


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

void AppUpdater::on_theme_changed(GtkSettings *settings, GParamSpec *pspec, GtkIconTheme *icon_theme)
{
    for (DockItem* item:m_dockitems) {
        setIconByTheme(item);
    }
}


