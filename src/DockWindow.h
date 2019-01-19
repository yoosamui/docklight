/* 
 * File:   MonitorGeometry.h
 * Author: yoo
 *
 * Created on November 4, 2018, 12:45 PM
 */

#ifndef MONITORGEOMETRY_H
#define	MONITORGEOMETRY_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <gtkmm.h>
#include "Configuration.h"
#include "AppWindow.h"


class AppWindow;

inline namespace DockWindow {
    int init(Gtk::Window* window);
    int get_DockWindowWidth();
    int get_DockWindowHeight();
    void update();
    GdkRectangle get_geometry();
    GdkRectangle get_workarea();
    void updateGeometry();
    void updateStrut();
    void removeStrut();
    void showDockWindow();
    guint get_dockWindowStartEndMargin();
}

/*
class DockWindow : public AppWindow {
public:

    DockWindow() : AppWindow() {

    }

    int postInit() {

        GdkScreen *screen = gdk_screen_get_default();

        g_signal_connect(G_OBJECT(screen), "monitors-changed",
                G_CALLBACK(monitor_changed_callback),
                (gpointer) this);


       

        updateStrut();
        showDockWindow();
        return 0;
    }

    void update(bool mode)
    {
        int itemsSize = this->m_dockpanel.get_dockItemsHeight() + this->get_dockWindowStartEndMargin();
        g_print("aaaaaaaaaaaaaaaaaa %d\n",itemsSize );
        
        this->showDockWindow();
    }
    void showDockWindow() {

        auto geometry = m_screen.get_PrimaryMonitor()->geometry;
        auto workarea = m_screen.get_PrimaryMonitor()->workarea;
        auto areaSize = Configuration::get_dockWindowSize();
        auto location = Configuration::get_dockWindowLocation();
        switch (location) {
            case panel_locationType::LEFT:
            case panel_locationType::RIGHT:
            {
                if (Configuration::is_panelMode()) {
                    this->resize(areaSize, geometry.height);
                    this->move((geometry.x + geometry.width) - areaSize, geometry.y);
                    break;
                }

                int itemsSize = this->m_dockpanel.get_dockItemsHeight() + this->get_dockWindowStartEndMargin();

                int center = (geometry.height / 2);
                int startY = center - (itemsSize / 2);

                this->resize(areaSize, itemsSize);

                if (location == panel_locationType::LEFT) {
                    this->move(geometry.x, geometry.y + startY);
                } else {
                    this->move((geometry.x + geometry.width) - areaSize, geometry.y + startY);
                }


                break;
            }
        }

    }

    void updateGeometry() {
        m_screen.update();
    }

    void updateStrut() {

    }

    void removeStrut() {

    }

    GdkRectangle get_geometry() {
        this->m_screen.get_PrimaryMonitor()->geometry;
    }

    GdkRectangle get_workarea() {
        this->m_screen.get_PrimaryMonitor()->workarea;
    }

    guint get_dockWindowStartEndMargin() {
        return 40;
    }

    int getDockWindowWidth() {
        
        auto test = "";
        return this->get_width();
    }

    int getDockWindowHeight() {
        return this->get_height();
    }

private:

    static void monitor_changed_callback(GdkScreen *screen, gpointer gtkwindow) {

        // Sleep for 2 seconds to give the monitor time to update the geometry
//        using namespace std::chrono_literals;
//        auto start = std::chrono::high_resolution_clock::now();
//        std::this_thread::sleep_for(2s);

        DockWindow o;

        o.removeStrut();
        o.updateGeometry();


        o.showDockWindow();
        o.updateStrut();
    }

};
 */

namespace DockWindowX {


    /**
     * Initialize the window
     * @param window the main window.
     * @return 0 is success -1 if any error.
     */
    int init(Gtk::Window* window);


    /**
     * Reserve screen (STRUT) space and dock the window
     * @return 0 = success or -1 error
     */
    int updateStrut(int size);
    int updateStrut();
    void removeStrut();

    /**
     * Place the window in the curremt strut.
     * @return 
     */
    void show();

    /**
     * Hide the window from the current strut.
     */
    void hide();

    //   namespace Monitor
    //    {
    //       
    //        GdkRectangle get_geometry();
    //       
    //        
    //        
    //    }
    guint get_dockWindowStartEndMargin();
    void move(int x, int y);
    // / unsigned int get_dockWindowSize();
    GdkRectangle get_geometry();

    //

    int getDockWindowWidth();
    int getDockWindowHeight();

    bool is_visible();

    void set_visible(bool visible);





    void updateGeometry();

    //    namespace DockWindow {
    //        unsigned int get_center();
    //        unsigned int get_size();
    //
    //
    //
    //    }





}


#endif	/* MONITORGEOMETRY_H */

