/*
 * File:   WckHandler.cpp
 * Author: yoo
 *
 * Created on March 18, 2019, 6:01 PM
 */

#include "WnckHandler.h"

namespace WnckHandler
{

    std::string get_windowName(WnckWindow* window)
    {
        return std::string(wnck_window_get_name(window));
    }

    void HomeCloseAllWindows()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }


            wnck_window_close(window, gtk_get_current_event_time());

        }
    }

    void HomeCloseAllExceptActive()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            if (wnck_window_is_active(window))
                continue;

            wnck_window_close(window, gtk_get_current_event_time());

        }
    }


    void HomeMinimizeAllExceptActive()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            if (wnck_window_is_active(window))
                continue;

            wnck_window_minimize(window);

        }
    }


    void HomeMinimizeAll()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            wnck_window_minimize(window);

        }
    }



    void HomeUnMinimizeAll()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            auto ct = gtk_get_current_event_time();
            if (wnck_window_is_minimized(window))
                wnck_window_unminimize(window, ct);

            wnck_window_activate(window, ct);

        }
    }


    bool isExistsUnMaximizedWindowsByDockItem(DockItem* dockitem)
    {
        bool result = false;
        for (auto item : dockitem->m_items) {
            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            if (wnck_window_is_minimized(window) == FALSE) {
                result = true;

                break;
            }

        }

        return result;
    }


    bool isExistsMinimizedWindowsByDockItem(DockItem* dockitem)
    {
        bool result = false;
        for (auto item : dockitem->m_items) {
            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;


            if (wnck_window_is_minimized(window)) {
                result = true;
                break;
            }

        }

        return result;
    }



    void closeAllExceptActiveByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {
            if (item->m_window == NULL)
                continue;
            if (wnck_window_is_active(item->m_window))
                continue;

            wnck_window_close(item->m_window, gtk_get_current_event_time());
        }
    }


    void closeAllByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {
            if (item->m_window == NULL)
                continue;

            closeByWindow(item->m_window);
        }
    }

    void closeByWindow(WnckWindow* window)
    {
            wnck_window_minimize(window);
            wnck_window_close(window, gtk_get_current_event_time());
    }

    void minimizeAllExceptActiveByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {

            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            if (wnck_window_is_active(item->m_window))
                continue;

            if (wnck_window_is_minimized(window) == false)
                wnck_window_minimize(window);
        }
    }


    void minimizeAllByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {

            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            if (wnck_window_is_minimized(window) == false)
                wnck_window_minimize(window);
        }
    }


    void unMinimizeAllByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {

            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            auto ct = gtk_get_current_event_time();
            if (wnck_window_is_minimized(window))
                wnck_window_unminimize(window, ct);

            wnck_window_activate(window, ct);

        }
    }





    bool isExitsActivetWindowByDockItem(DockItem* dockitem)
    {
        for (auto item : dockitem->m_items) {
            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;


            if (wnck_window_is_active(window))
                return true;
        }

        return false;
    }



    int isExitstWindowsByDockItem(DockItem* dockitem)
    {
        int count = 0;
        for (auto item : dockitem->m_items) {
            WnckWindow *window = item->m_window;
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            count++;
        }

        return count;
    }


    WnckWindow* get_ExistingWindowDock()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);
        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            auto instance = wnck_window_get_class_instance_name(window);
            auto wname = wnck_window_get_name(window);

            //   g_print("%s %s\n", instance, wname);
            //
            //            WnckWindowType wt = wnck_window_get_window_type(window);
            //
            //            if (wt != WNCK_WINDOW_DOCK )
            //            {
            //
            //                continue;
            //            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }
            //  return window;
        }
        return NULL;

    }

    /**
     * Gets the active WnckWindow on screen . May return NULL sometimes, since
     * not all window managers guarantee that a window is always active.
     * @return true if any window is in fullscreen otherwise false;
     */
    bool FullscreenActive()
    {
        WnckWindow *wckwindow = wnck_screen_get_active_window(wnck_screen_get_default());
        if (wckwindow != NULL && wnck_window_is_fullscreen(wckwindow)) {
            return true;
        }

        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            wckwindow = WNCK_WINDOW(window_l->data);
            if (wckwindow == NULL)
                continue;

            if (wnck_window_is_fullscreen(wckwindow)) {
                return true;
            }
        }
        return false;
    }
    int windowscount()
    {
        int count = 0;
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {
            //
            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }

            count++;
        }

        return count;
    }



    //int getWindowsByName(const std::string& appname, std::vector<windowData>& data)
    //{
    //WnckScreen *screen;
    //GList *window_l;
    //struct windowData st;
    //screen = wnck_screen_get_default();
    //wnck_screen_force_update(screen);

    //for (window_l = wnck_screen_get_windows(screen);
    //window_l != NULL; window_l = window_l->next) {

    //WnckWindow *window = WNCK_WINDOW(window_l->data);
    //if (window == NULL)
    //continue;

    //WnckWindowType wt = wnck_window_get_window_type(window);

    //if (wt == WNCK_WINDOW_DESKTOP ||
    //wt == WNCK_WINDOW_DOCK ||
    //wt == WNCK_WINDOW_TOOLBAR ||
    //wt == WNCK_WINDOW_MENU) {

    //continue;
    //}

    //std::string the_appname;
    //std::string the_instancename;
    //std::string the_groupname;
    //std::string the_titlename;

    //if (Launcher::getAppNameByWindow(window,
    //the_appname,
    //the_instancename,
    //the_groupname,
    //the_titlename) == FALSE) {
    //continue;
    //}


    //if (the_groupname == appname) {
    //st.window = window;
    //strcpy(st.appname,the_groupname.c_str());
    //strcpy(st.titlename,the_appname.c_str());

    //data.push_back(st);
    //}

    //}

    //return data.size();
    //}




    //WnckWindow* getWindowByName(const std::string& appname, std::string& tittle)
    //{
    //WnckScreen *screen;
    //GList *window_l;

    //screen = wnck_screen_get_default();
    //wnck_screen_force_update(screen);

    //for (window_l = wnck_screen_get_windows(screen);
    //window_l != NULL; window_l = window_l->next) {

    //WnckWindow *window = WNCK_WINDOW(window_l->data);
    //if (window == NULL)
    //continue;

    //WnckWindowType wt = wnck_window_get_window_type(window);

    //if (wt == WNCK_WINDOW_DESKTOP ||
    //wt == WNCK_WINDOW_DOCK ||
    //wt == WNCK_WINDOW_TOOLBAR ||
    //wt == WNCK_WINDOW_MENU) {

    //continue;
    //}

    //std::string the_appname;
    //std::string the_instancename;
    //std::string the_groupname;
    //std::string the_titlename;

    //if (Launcher::getAppNameByWindow(window,
    //the_appname,
    //the_instancename,
    //the_groupname,
    //the_titlename) == FALSE) {
    //return nullptr;
    //}

    //if (the_groupname == appname) {
    //tittle = the_appname;
    //return window;
    //}

    //}

    //return nullptr;
    //}




    WnckWindow* get_Active()
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            if (wnck_window_is_active(window)) {
                return window;
            }
        }

        return nullptr;
    }

    bool isWindowExists(XID xid)
    {
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            if (wnck_window_get_xid(window) == xid)
                return true;

        }

        return false;
    }


    bool isExistsMinimizedWindows()
    {

        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }

            if (wnck_window_is_minimized(window))
                return true;
        }

        return false;
    }






    int minimizedWindowscount()
    {
        int count = 0;
        WnckScreen *screen;
        GList *window_l;

        screen = wnck_screen_get_default();
        wnck_screen_force_update(screen);

        for (window_l = wnck_screen_get_windows(screen);
                window_l != NULL; window_l = window_l->next) {

            WnckWindow *window = WNCK_WINDOW(window_l->data);
            if (window == NULL)
                continue;

            WnckWindowType wt = wnck_window_get_window_type(window);

            if (wt == WNCK_WINDOW_DESKTOP ||
                    wt == WNCK_WINDOW_DOCK ||
                    wt == WNCK_WINDOW_TOOLBAR ||
                    wt == WNCK_WINDOW_MENU) {

                continue;
            }

            const char* instancename = wnck_window_get_class_instance_name(window);
            if (instancename != NULL && strcmp(instancename, DOCKLIGHT_INSTANCENAME) == 0) {
                continue;
            }

            if (wnck_window_is_minimized(window))
                count++;
        }

        return count;
    }


    int unMinimizedWindowsCount()
    {
        return windowscount() - minimizedWindowscount();
    }

    void hideWindow(Gtk::Window* instance)
    {
        instance->hide();
    }





    /////////////////////////


    void SelectWindow(WnckWindow* window)
    {
        if (window == NULL)
            return;


        int ct = gtk_get_current_event_time();
        wnck_window_activate(window, ct);

        if (wnck_window_is_minimized(window))
            wnck_window_unminimize(window, ct);
    }

    /**
     * Active the current Active window
     * @param window
     */
    void ActivateWindow(WnckWindow* window)
    {
        if (window == NULL)
            return;

        if (wnck_window_is_active(window)) {
            wnck_window_minimize(window);
            return;
        }

        int ct = gtk_get_current_event_time();
        wnck_window_activate(window, ct);

        if (wnck_window_is_minimized(window))
            wnck_window_unminimize(window, ct);
    }

    /**
     * check if any active window in cild items exists.
     * if yes the window pointer will return otherwise null.
     * @param the child items.
     * @return the window pointer will return when an active  window is found otherwise null.
     */
    WnckWindow* get_ActiveWindowIfAny(DockItem* item)
    {
        WnckWindow* window = nullptr;

        for (DockItem* citem:item->m_items) {

            if (citem->m_window == nullptr) {
                continue;
            }

            if (!wnck_window_is_active(citem->m_window)) {
                continue;
            }

            window = citem->m_window;
        }

        return window;
    }
}
