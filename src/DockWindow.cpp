/* 
 * File:   MonitorGeometry.cpp
 * Author: yoo
 * 
 * Created on November 4, 2018, 12:45 PM
 */

#include "Configuration.h"


//#include <gtk-3.0/gdk/gdkdisplay.h>
#include <gdk/gdk.h>
//#include <X11/Xlib.h> 

#include "DockWindow.h"
#include "WindowControl.h"
#include "AppWindow.h"
#include "Screen.h"
#include "Struts.h"

inline namespace DockWindow
{
    Gtk::Window* m_window = nullptr;

    int init(Gtk::Window* window)
    {
        m_window = window;
        updateStrut();
        showDockWindow();
        return 0;
    }

    int get_DockWindowWidth()
    {
        if (m_window != nullptr)
            return m_window->get_width();
    }

    int get_DockWindowHeight()
    {
        if (m_window != nullptr)
            return m_window->get_height();
    }

    guint get_dockWindowStartEndMargin()
    {
        return 40;
    }

    void update()
    {
        showDockWindow();
    }

    GdkRectangle get_geometry()
    {
        if (m_window != nullptr)
            ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->geometry;
    }

    GdkRectangle get_workarea()
    {
        if (m_window != nullptr)
            ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->workarea;
    }

    void updateGeometry()
    {
        if (m_window != nullptr)
            ((AppWindow*)m_window)->m_screen.update();
    }

    void updateStrut()
    {
        if (Configuration::is_activateStrut()) {
            if (m_window != nullptr)
                ((AppWindow*)m_window)->m_screen.struts->update();
        }
    }

    void removeStrut()
    {
        if (Configuration::is_activateStrut()) {
            if (m_window != nullptr)
                ((AppWindow*)m_window)->m_screen.struts->remove();
        }
    }

    void showDockWindow()
    {
        if (m_window == nullptr) {
            return;
        }

        g_print("showDockWindow\n");
        auto geometry = ((AppWindow*)m_window)->m_screen.get_PrimaryMonitor()->geometry;
        auto areaSize = Configuration::get_dockWindowSize();
        auto location = Configuration::get_dockWindowLocation();
        switch (location)
        {
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(areaSize, geometry.height);
                    m_window->move((geometry.x + geometry.width) - areaSize, geometry.y);
                    break;
                }

                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsWidth() +  get_dockWindowStartEndMargin();
                
                int center = (/*(geometry.x +*/ geometry.width / 2);
                int startX = center - (itemsSize / 2);

                m_window->resize(itemsSize, areaSize);

                if (location == panel_locationType::TOP) {
                   // m_window->move(workarea.x + startX, workarea.y);
                     m_window->move(geometry.x + startX, geometry.y + Configuration::set_WindowDockMonitorMargin_Top());
                }
                else {

                    areaSize += Configuration::set_WindowDockMonitorMargin_Bottom();
                    m_window->move(geometry.x + startX, (geometry.y + geometry.height) - areaSize);
                      
                }


                break;
            }
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(areaSize, geometry.height);
                    m_window->move((geometry.x + geometry.width) - areaSize, geometry.y);
                    break;
                }

                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsHeight() + get_dockWindowStartEndMargin();

                int center = (geometry.height / 2);
                int startY = center - (itemsSize / 2);

                m_window->resize(areaSize, itemsSize);

                if (location == panel_locationType::LEFT) {
                    m_window->move(geometry.x + Configuration::set_WindowDockMonitorMargin_Left(), geometry.y + startY);
                }
                else {
                    areaSize += Configuration::set_WindowDockMonitorMargin_Right();
                    m_window->move((geometry.x + geometry.width) - areaSize, geometry.y + startY);
                }


                break;
            }
        }

    }
}

namespace DockWindowX
{

    Screen* m_screen = NULL;

    // Struts* m_struts;

    //    class Monitor
    //    {
    //    public:
    //        GdkMonitor* monitor;
    //        GdkRectangle workarea;
    //        GdkRectangle geometry;
    //        int index;
    //        bool primary = false;
    //    };


    //    std::vector<Monitor*> monitorlist;
    //    GdkRectangle geometry;
    //    GdkRectangle workarea;
    //
    //    int m_monitors;
    Gtk::Window* m_window = nullptr;
    //    GdkWindow* m_gdk_window = nullptr;
    bool initSet = false;
    panel_locationType m_location;
    unsigned int m_areaSize;
    // int m_dockItemSize;
    bool m_visible = true;

    /**
     * Initialize the window
     * @param window the main window.
     * @return 0 is success -1 if any error.
     */
    int init(Gtk::Window* window)
    {
        if (initSet) {
            g_warning(" MonitorGeometry::init already set.");
            return -1;
        }

        if (window == NULL) {
            g_critical(" MonitorGeometry::update: window is null.");
            return -1;
        }
        if (m_screen == NULL) {
            m_screen->init(window);
        }
        /*
                // m_struts = new Struts(window);

                //   m_dockItemSize =DockWindow::get_size();//Configuration::get_itemSize();

                //        auto toplevel = gtk_window_new (GTK_WINDOW_TOPLEVEL);
                // auto  m_gdk_window = gtk_widget_get_window(toplevel);

                GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(window->gobj()));

                m_gdk_window = gtk_widget_get_window(toplevel);
                if (m_gdk_window == NULL) {
                    g_critical(" MonitorGeometry::update: gdk_window is null.");
                    return -1;
                }
                //auto screen = Gdk::Screen::get_default();
                //GdkScreen *defaultscreen = gdk_screen_get_default();
                //gint dm = gdk_screen_get_primary_monitor(defaultscreen);
                //screen->get_monitor_geometry(dm, monitor_geo);

                // Retrieves the Gdk::Rectangle representing the size and position of the 
                // individual monitor within the entire screen area. 
                GdkDisplay *defaultdisplay = gdk_display_get_default();
                if (defaultdisplay == NULL) {
                    g_critical(" AppWindow::Init: there is no default display.");
                    return -1;
                }


                GdkDisplay *display = gdk_display_get_default();
                m_monitors = gdk_display_get_n_monitors(display);
                GdkMonitor *monitor = gdk_display_get_primary_monitor(defaultdisplay);
                if (monitor == NULL) {
                    g_critical(" AppWindow::Init: there is no primary monitor configured by the user..");
                    return -1;
                }

                // Retrieves the size and position of the “work area” on a monitor within the display coordinate space.
                // The returned geometry is in ”application pixels”, not in ”device pixels” (see gdk_monitor_get_scale_factor()).
                // The work area should be considered when positioning menus and similar popups, to avoid placing them below panels,
                // docks or other desktop components.
                // Note that not all backends may have a concept of workarea. 
                // This function will return the monitor geometry if a workarea is not available, or does not apply.
                gdk_monitor_get_workarea(monitor, &workarea);



                // GdkRectangle to be filled with the monitor geometry
                gdk_monitor_get_geometry(monitor, &geometry);
        
         */
        auto wncWindow = WindowControl::get_ExistingWindowDock();

        m_location = Configuration::get_dockWindowLocation();
        m_areaSize = Configuration::get_dockWindowSize();
        m_window = window;
        initSet = true;

        return 0; //updateStrut(m_areaSize);
    }

    GdkRectangle get_geometry()
    {
        GdkRectangle workarea;
        return workarea;



        return m_screen->get_PrimaryMonitor()->geometry;

    }

    bool is_visible()
    {
        // int x, y;
        //m_window->get_position(x, y);

        //  return y == (geometry.height - Configuration::get_dockWindowSize());
        //  y >= DockWindow::get_geometry().height - 6;


        return m_visible;
    }

    void set_visible(bool visible)
    {
        m_visible = visible;
    }
    //    namespace Monitor {
    //
    //        GdkRectangle get_geometry() {
    //
    //            GdkRectangle geometry;
    //
    //            m_window->get_position(geometry.x, geometry.y);
    //            geometry.height = m_window->get_width();
    //            geometry.height = m_window->get_height();
    //
    //            return geometry;
    //        }
    //    }

    //    unsigned int get_center()
    //    {
    //        unsigned int result = -1;
    //        switch (m_location)
    //        {
    //            case panel_locationType::TOP:
    //            case panel_locationType::BOTTOM:
    //                result = geometry.width / 2;
    //                break;
    //            case panel_locationType::LEFT:
    //            case panel_locationType::RIGHT:
    //                result = geometry.height / 2;
    //                break;
    //            default:
    //                break;
    //        }
    //    }
    //
    //    unsigned int get_centerX()
    //    {
    //        unsigned int result = -1;
    //        switch (m_location)
    //        {
    //            case panel_locationType::TOP:
    //            case panel_locationType::BOTTOM:
    //                result = geometry.width / 2;
    //                break;
    //            case panel_locationType::LEFT:
    //            case panel_locationType::RIGHT:
    //                result = geometry.height / 2;
    //                break;
    //            default:
    //                break;
    //        }
    //    }

    //    unsigned int get_dockWindowSize()
    //    {
    //        
    //        return ((AppWindow*)m_window)->m_dockpanel.get_dockItemsHeight()+
    //               get_dockWindowStartEndMargin();
    //    }

    guint get_dockWindowStartEndMargin()
    {
        return 40;
    }

    void updateGeometry()
    {
        // initSet = false;
        // init(m_window);
        m_screen->update();

    }

    int getDockWindowWidth()
    {
        return m_window->get_width();
    }

    int getDockWindowHeight()
    {
        // g_print("%d\n", m_window->get_width());
        return m_window->get_height();
    }

    /**
     * update the window position and store the monitor geometry
     * Reserve screen (STRUT) space and dock the window
     * @param window
     * @return 0 = success or -1 error
     */
    int updateStrut()
    {
        m_screen->update();
        if (Configuration::is_activateStrut()) {
            m_screen->struts->update();
        }
    }

    void removeStrut()
    {
        if (Configuration::is_activateStrut()) {
            m_screen->struts->remove();
        }
    }

    /*
    int updateStrut(int m_areaSize)
    {
        return 0;
        // # (d) reserve space (a "strut") for the bar so it does not become obscured
        // #     when other windows are maximized, etc
        // update strut
        long insets[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        if (m_areaSize > 1) {




            //https://ubuntu-mate.community/t/pantheon-on-laptop-external-monitor-windows-created-under-panel/17631/3
            //https://alice-at-ubuntu.blogspot.com/2015/01/how-to-reserve-space-for-panel-between.html
            //https://stackoverflow.com/questions/21384819/x11-struts-geometry




            auto display = gdk_display_get_default();
            m_monitors = gdk_display_get_n_monitors(display);
            monitorlist.clear();

            for (int i = 0; i < m_monitors; i++) {

                Monitor* m = new Monitor();

                m->monitor = gdk_display_get_monitor(display, i);
                m->index = i;

                gdk_monitor_get_geometry(m->monitor, &m->geometry);
                gdk_monitor_get_workarea(m->monitor, &m->workarea);

                if (gdk_monitor_is_primary(m->monitor)) {
                    m->primary = true;

                }
                monitorlist.push_back(m);
            }

            Monitor* primary = NULL;
            Monitor* next = NULL;

            for (auto m:monitorlist) {
                if (m->primary) {
                    primary = m;
                }
                else {
                    next = m;
                }
            }

            m_screen->update();
            switch (m_location)
            {
                case panel_locationType::TOP:

                    //                    
                    //                    if (m_monitors > 1) {
                    //                        //0,0,410,0,0,0,0,0,2560,4479,0,0
                    //                        if (primary->geometry.x > 0) {
                    //                            insets[strutsPosition::Top] = primary->workarea.y + m_areaSize;;
                    //                            insets[strutsPosition::TopStart] = next->geometry.width;
                    //                            insets[strutsPosition::TopEnd] = (primary->geometry.width + next->geometry.width) - 1;
                    //                            break;
                    //                        }
                    //                    }
                    //
                    //                    insets[strutsPosition::Top] = m_areaSize + primary->workarea.y;
                    //                    insets[strutsPosition::TopStart] = workarea.y;
                    //                    insets[strutsPosition::TopEnd] = geometry.height + primary->workarea.y;
                    //                    break;


                    if (m_screen->get_MonitorsCount() > 1) {
                        //0,0,410,0,0,0,0,0,2560,4479,0,0
                        if (m_screen->get_PrimaryMonitor()->geometry.x > 0) {
                            insets[strutsPosition::Top] =
                                    m_screen->get_PrimaryMonitor()->workarea.y + m_areaSize;

                            insets[strutsPosition::TopStart] =
                                    m_screen->get_NextMonitor()->geometry.width;

                            insets[strutsPosition::TopEnd] =
                                    (m_screen->get_PrimaryMonitor()->geometry.width +
                                    m_screen->get_NextMonitor()->geometry.width) - 1;
                            break;
                        }
                    }


                    insets[strutsPosition::Top] =
                            m_areaSize + m_screen->get_PrimaryMonitor()->workarea.y;

                    insets[strutsPosition::TopStart] =
                            m_screen->get_PrimaryMonitor()->workarea.y;

                    insets[strutsPosition::TopEnd] =
                            m_screen->get_PrimaryMonitor()->geometry.height +
                            m_screen->get_PrimaryMonitor()->workarea.y;
                    break;


                case panel_locationType::BOTTOM:
                {
                    Monitor* primary = NULL;
                    Monitor* next = NULL;

                    for (auto m:monitorlist) {
                        if (m->primary) {
                            primary = m;
                        }
                        else {
                            next = m;
                        }
                    }


                    if (m_monitors > 1) {
                        //0,0,0,50,0,0,0,0,0,0,2560,4479
                        if (primary->geometry.x > 0) {
                            insets[strutsPosition::Bottom] = m_areaSize;
                            insets[strutsPosition::BottomStart] = next->geometry.width;
                            insets[strutsPosition::BottomEnd] = (primary->geometry.width + next->geometry.width) - 1;
                            break;
                        }
                    }

                    //0,0,0,60,0,0,0,0,0,0,0,1440
                    insets[strutsPosition::Bottom] = m_areaSize;
                    insets[strutsPosition::BottomStart] = 0;
                    insets[strutsPosition::BottomEnd] = primary->geometry.width - 1;
                    break;
                }

                    break;

                case panel_locationType::LEFT:
                {

                    Monitor* primary = NULL;
                    Monitor* next = NULL;

                    for (auto m:monitorlist) {
                        if (m->primary) {
                            primary = m;
                        }
                        else {
                            next = m;
                        }
                    }


                    if (m_monitors > 1) {

                        //0,0,2560,0,0,0,0,0,2560,2600,0,0
                        if (primary->geometry.x > 0) {
                            insets[strutsPosition::Top] = next->geometry.width - 1;
                            insets[strutsPosition::TopStart] = next->geometry.width - 1;
                            insets[strutsPosition::TopEnd] = insets[strutsPosition::TopStart] + m_areaSize;
                            break;
                        }
                    }
                    insets[strutsPosition::Left] = m_areaSize;
                    insets[strutsPosition::LeftStart] = primary->geometry.x - 1;
                    insets[strutsPosition::LeftEnd] = primary->geometry.x + primary->geometry.height;

                    break;

                }



                case panel_locationType::RIGHT:
                {
                    Monitor* primary = NULL;
                    Monitor* next = NULL;

                    for (auto m:monitorlist) {
                        if (m->primary) {
                            primary = m;
                        }
                        else {
                            next = m;
                        }
                    }

                    if (m_monitors > 1) {
                        // 0,60,0,0,0,0,0,1080,0,0,0,0
                        if (primary->geometry.x > 0) {
                            insets[strutsPosition::Right] = m_areaSize;
                            insets[strutsPosition::RightStart] = 0;
                            insets[strutsPosition::RightEnd] = next->geometry.width - 1;
                            break;
                        }
                    }

                    // at this point this is the firts horizotal monitor
                    if (next->geometry.x == 0) {
                        //0,60,0,0,0,0,0,1080,0,0,0,0
                        insets[strutsPosition::Right] = m_areaSize;
                        insets[strutsPosition::RightStart] = 0;
                        insets[strutsPosition::RightEnd] = next->geometry.height - 1;
                        break;
                    }

                    //0,0,2560,0,0,0,0,0,2500,2550,0,0
                    insets[strutsPosition::Top] = primary->geometry.width - 1;
                    insets[strutsPosition::TopStart] = primary->geometry.width - m_areaSize;
                    insets[strutsPosition::TopEnd] = insets[strutsPosition::TopStart] + m_areaSize;
                    break;
                }
            }
        }

        gdk_property_change(m_gdk_window,
                            gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                            gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                            (unsigned char *)&insets, 12);


        gdk_property_change(m_gdk_window,
                            gdk_atom_intern("_NET_WM_STRUT", FALSE),
                            gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                            (unsigned char *)&insets, 4);


        return 0;
    }

    void removeStrut(){
        // m_struts->update(true);
        //updateStrut(0);
    }
     */
    void show()
    {
        auto geometry = m_screen->get_PrimaryMonitor()->geometry;
        auto workarea = m_screen->get_PrimaryMonitor()->workarea;

        switch (m_location)
        {
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
            {



                if (Configuration::is_panelMode()) {
                    m_window->resize(m_areaSize, geometry.height);
                    m_window->move((geometry.x + geometry.width) - m_areaSize, geometry.y);
                    break;
                }

                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsWidth();
                itemsSize += get_dockWindowStartEndMargin();
                int center = (/*(geometry.x +*/ geometry.width / 2);
                int startX = center - (itemsSize / 2);

                m_window->resize(itemsSize, m_areaSize);

                if (m_location == panel_locationType::TOP) {
                    m_window->move(geometry.x + startX, workarea.y);

                }
                else {

                    m_window->move(geometry.x + startX, (geometry.y + geometry.height) - m_areaSize);
                }


                break;
            }
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(m_areaSize, geometry.height);
                    m_window->move((geometry.x + geometry.width) - m_areaSize, geometry.y);
                    break;
                }

                int itemsSize = ((AppWindow*)m_window)->m_dockpanel.get_dockItemsHeight();
                itemsSize += get_dockWindowStartEndMargin();
                int center = (geometry.height / 2);
                int startY = center - (itemsSize / 2);

                m_window->resize(m_areaSize, itemsSize);

                if (m_location == panel_locationType::LEFT) {
                    m_window->move(geometry.x, geometry.y + startY);
                }
                else {

                    m_window->move((geometry.x + geometry.width) - m_areaSize, geometry.y + startY);
                }


                break;
            }

            default:
                break;
        }

    }

    /**
     * 
     */
    void hide()
    {


        //        switch (m_location)
        //        {
        //            case panel_locationType::BOTTOM:
        //            {
        //                if (Configuration::is_panelMode()) {
        //                    m_window->resize(geometry.width, m_areaSize);
        //                    m_window->move(geometry.x, geometry.height);
        //                    break;
        //                }
        //
        //                int width = get_dockWindowSize();
        //                m_window->resize(width, m_areaSize);
        //                m_window->move(((geometry.x + geometry.width) / 2) - width / 2, geometry.height);
        //            }
        //                break;
        //
        //            case panel_locationType::TOP:
        //            {
        //                if (Configuration::is_panelMode()) {
        //                    m_window->resize(geometry.width, m_areaSize);
        //                    m_window->move(geometry.x, -m_areaSize);
        //                    break;
        //                }
        //
        //                int width = get_dockWindowSize();
        //                m_window->resize(width, m_areaSize);
        //                m_window->move(((geometry.x + geometry.width) / 2) - width / 2, -m_areaSize);
        //                break;
        //            }
        //            case panel_locationType::LEFT:
        //            {
        //                if (Configuration::is_panelMode()) {
        //                    m_window->resize(m_areaSize, geometry.height);
        //                    m_window->move(geometry.x - m_areaSize, geometry.y);
        //                    break;
        //                }
        //
        //                int height = get_dockWindowSize();
        //                m_window->resize(m_areaSize, height);
        //                m_window->move(geometry.x - m_areaSize, ((geometry.y + geometry.height) / 2) - height / 2);
        //                //     posY = ((geometry.y + geometry.height) / 2) - height / 2;
        //                break;
        //            }
        //            case panel_locationType::RIGHT:
        //            {
        //                if (Configuration::is_panelMode()) {
        //                    m_window->resize(m_areaSize, geometry.height);
        //                    m_window->move(geometry.x + geometry.width, geometry.y);
        //                    break;
        //                }
        //
        //                int height = get_dockWindowSize();
        //                m_window->resize(m_areaSize, height);
        //                m_window->move(geometry.x - geometry.width, ((geometry.y + geometry.height) / 2) - height / 2);
        //                break;
        //            }
        //            default:
        //                break;
        //        }

        set_visible(false);
    }


}
