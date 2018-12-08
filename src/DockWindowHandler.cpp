/* 
 * File:   MonitorGeometry.cpp
 * Author: yoo
 * 
 * Created on November 4, 2018, 12:45 PM
 */

#include "DockWindowHandler.h"
#include "WindowControl.h"
#include "AppWindow.h"


namespace DockWindow
{
    GdkRectangle geometry;
    Gtk::Window* m_window = nullptr;
    GdkWindow* m_gdk_window = nullptr;
    bool initSet = false;
    panel_locationType m_location;
    unsigned int m_areaSize;
    int m_dockItemSize;
    bool m_visible = true;

    GdkRectangle get_geometry()
    {
        return geometry;

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

    unsigned int get_center()
    {
        unsigned int result = -1;
        switch (m_location)
        {
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
                result = geometry.width / 2;
                break;
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
                result = geometry.height / 2;
                break;
            default:
                break;
        }
    }

    unsigned int get_centerX()
    {
        unsigned int result = -1;
        switch (m_location)
        {
            case panel_locationType::TOP:
            case panel_locationType::BOTTOM:
                result = geometry.width / 2;
                break;
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
                result = geometry.height / 2;
                break;
            default:
                break;
        }
    }

    unsigned int get_dockSize()
    {
        return ((AppWindow*)m_window)->m_dockpanel.get_dockItemsWidth();
    }

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

        m_dockItemSize = Configuration::get_itemSize();

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

        GdkMonitor *monitor = gdk_display_get_primary_monitor(defaultdisplay);
        if (monitor == NULL) {
            g_critical(" AppWindow::Init: there is no primary monitor configured by the user..");
            return -1;
        }

        // GdkRectangle to be filled with the monitor geometry
        gdk_monitor_get_geometry(monitor, &geometry);
        auto wncWindow = WindowControl::get_ExistingWindowDock();

        m_location = Configuration::get_dockWindowLocation();
        m_areaSize = Configuration::get_dockWindowSize();
        m_window = window;
        initSet = true;

        return 0; //updateStrut(m_areaSize);
    }

    unsigned int getDockItemWidth()
    {
        return m_window->get_width();
    }

    unsigned int getDockItemHeight()
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
        if (!initSet) {
            return 0;
        }
        return updateStrut(m_areaSize);
    }

    int updateStrut(int m_areaSize)
    {





        //   m_window->show_all();
        // # (d) reserve space (a "strut") for the bar so it does not become obscured
        // #     when other windows are maximized, etc
        // update strut
        long insets[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        // const int size = 100;

        // showDockWindow();

        switch (m_location)
        {
            case panel_locationType::BOTTOM:
                //   m_window->resize(geometry.width, m_areaSize);

                insets[strutsPosition::BottomStart] = geometry.y;
                insets[strutsPosition::BottomEnd] = geometry.height;
                insets[strutsPosition::Bottom] = m_areaSize;
                break;

            case panel_locationType::TOP:
                // m_window->resize(geometry.width, m_areaSize);

                insets[strutsPosition::TopStart] = 0; //geometry.y;
                insets[strutsPosition::TopEnd] = geometry.height;
                insets[strutsPosition::Top] = m_areaSize;
                break;

            case panel_locationType::LEFT:
                //m_window->resize(m_areaSize, geometry.height);

                insets[strutsPosition::LeftStart] = geometry.x;
                insets[strutsPosition::LeftEnd] = geometry.width;
                insets[strutsPosition::Left] = m_areaSize;
                break;

            case panel_locationType::RIGHT:
                //m_window->resize(m_areaSize, geometry.height);

                insets[strutsPosition::RightStart] = geometry.x;
                insets[strutsPosition::RightEnd] = geometry.width;
                insets[strutsPosition::Right] = m_areaSize;
                break;

            default:
                break;
        }
        //https://developer.gnome.org/gtk-tutorial/stable/x2431.html
        //  m_window->set_property()

        // m_window->set_p
        //auto topw = m_window->get_toplevel();
        // we set _NET_WM_STRUT, the older mechanism as well as _NET_WM_STRUT_PARTIAL
        gdk_property_change(m_gdk_window,
                            gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                            gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                            (unsigned char *)&insets, 12);


        gdk_property_change(m_gdk_window,
                            gdk_atom_intern("_NET_WM_STRUT", FALSE),
                            gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                            (unsigned char *)&insets, 4);



        //m_window->property_c
        //  GdkWindow::property_change()


        return 0;
    }

    void removeStrut()
    {
        updateStrut(1);
    }

    void show()
    {
        switch (m_location)
        {
            case panel_locationType::BOTTOM:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(geometry.width, m_areaSize);
                    m_window->move(geometry.x, geometry.height - m_areaSize);
                    break;
                }
                int width = get_dockSize();
                m_window->resize(width, m_areaSize);
                m_window->move(((geometry.x + geometry.width) / 2) - width / 2, geometry.height - m_areaSize);
            }
                break;
                //
                //            case panel_locationType::TOP:
                //                m_window->resize(geometry.width, m_areaSize);
                //                m_window->move(geometry.x, geometry.y);
                //                break;
                //            case panel_locationType::LEFT:
                //                m_window->resize(m_areaSize, geometry.height);
                //                m_window->move(geometry.x, geometry.y);
                //                break;
                //
                //            case panel_locationType::RIGHT:
                //                m_window->resize(m_areaSize, geometry.height);
                //                m_window->move(geometry.x + (geometry.width / 2), geometry.y);
                //                break;
                
            case panel_locationType::TOP:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(geometry.width, m_areaSize);
                    m_window->move(geometry.x, geometry.y);
                    break;
                }

                int width = get_dockSize();
                m_window->resize(width, m_areaSize);
                m_window->move(((geometry.x + geometry.width) / 2) - width / 2, geometry.y);
                break;
            }
            case panel_locationType::LEFT:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(m_areaSize, geometry.height);
                    m_window->move(geometry.x, geometry.y);
                    break;
                }

                int height = get_dockSize();
                m_window->resize(m_areaSize, height);
                m_window->move( geometry.x, ((geometry.y + geometry.height) / 2) - height / 2);
                //     posY = ((geometry.y + geometry.height) / 2) - height / 2;
                break;
            }
            case panel_locationType::RIGHT:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(m_areaSize, geometry.height);
                    m_window->move(geometry.x + geometry.width, geometry.y);
                    break;
                }

                int height = get_dockSize();
                m_window->resize(m_areaSize, height);
                m_window->move((geometry.x + geometry.width) - m_areaSize, ((geometry.y + geometry.height) / 2) - height / 2);
                break;
            }

            default:
                break;
        }

        set_visible(true);
    }

    void move(int x, int y)
    {
        int posX = x;
        int posY = y;

        switch (m_location)
        {
            case panel_locationType::BOTTOM:
            case panel_locationType::TOP:

            {
                if (Configuration::is_panelMode() == false) {
                    int width = get_dockSize();
                    posX = ((geometry.x + geometry.width) / 2) - width / 2;
                    posY = y;
                }
                m_window->move(posX, posY);

            }
                break;

            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
            {
                if (Configuration::is_panelMode() == false) {
                    int height = get_dockSize();
                    posX = x;
                    posY = ((geometry.y + geometry.height) / 2) - height / 2;
                }
                m_window->move(posX, posY);

            }
                break;


        }
    }

    /**
     * 
     */
    void hide()
    {


        switch (m_location)
        {
            case panel_locationType::BOTTOM:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(geometry.width, m_areaSize);
                    m_window->move(geometry.x, geometry.height);
                    break;
                }

                int width = get_dockSize();
                m_window->resize(width, m_areaSize);
                m_window->move(((geometry.x + geometry.width) / 2) - width / 2, geometry.height);
            }
                break;

            case panel_locationType::TOP:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(geometry.width, m_areaSize);
                    m_window->move(geometry.x, -m_areaSize);
                    break;
                }

                int width = get_dockSize();
                m_window->resize(width, m_areaSize);
                m_window->move(((geometry.x + geometry.width) / 2) - width / 2, -m_areaSize);
                break;
            }
            case panel_locationType::LEFT:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(m_areaSize, geometry.height);
                    m_window->move(geometry.x - m_areaSize, geometry.y);
                    break;
                }

                int height = get_dockSize();
                m_window->resize(m_areaSize, height);
                m_window->move(geometry.x - m_areaSize, ((geometry.y + geometry.height) / 2) - height / 2);
                //     posY = ((geometry.y + geometry.height) / 2) - height / 2;
                break;
            }
            case panel_locationType::RIGHT:
            {
                if (Configuration::is_panelMode()) {
                    m_window->resize(m_areaSize, geometry.height);
                    m_window->move(geometry.x + geometry.width, geometry.y);
                    break;
                }

                int height = get_dockSize();
                m_window->resize(m_areaSize, height);
                m_window->move(geometry.x - geometry.width, ((geometry.y + geometry.height) / 2) - height / 2);
                break;
            }
            default:
                break;
        }

        set_visible(false);
    }
}
