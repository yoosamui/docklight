/* 
 * File:   DockPanel.cpp
 * Author: yoo
 * 
 * Created on November 6, 2018, 7:31 PM
 */

#include "DockItem.h"


#include "DockPanel.h"
#include "DockWindow.h"
#include "Configuration.h"
#include "Launcher.h"
#include "Animations.h"
#include "AppWindow.h"
#include "DockWindow.h"

#include <glibmm/i18n.h>
#include <gtkmm/window.h>
#include <gtkmm/messagedialog.h>
#include <gdkmm/cursor.h>
#include <limits.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <cairo/cairo.h>

// static members
//std::vector<DockItem*> DockPanel::m_dockitems;
int DockPanel::m_currentMoveIndex;
//Gtk::Window* DockPanel::m_AppWindow;
//DockWindow* DockPanel::m_DockWindow;

//AppUpdater DockPanel::m_appUpdater;

DockPanel::DockPanel():

//m_previousCellwidth(DEF_CELLWIDTH),
m_homeiconFilePath(Utilities::getExecPath(DEF_ICONNAME))
{

    // Set masks for mouse events
    add_events(Gdk::BUTTON_PRESS_MASK |
               Gdk::BUTTON_RELEASE_MASK |
               Gdk::SCROLL_MASK |
               Gdk::SMOOTH_SCROLL_MASK |
               Gdk::POINTER_MOTION_HINT_MASK |
               Gdk::FOCUS_CHANGE_MASK |
               Gdk::ENTER_NOTIFY_MASK |
               Gdk::LEAVE_NOTIFY_MASK |
               Gdk::POINTER_MOTION_MASK);

    //    set_app_paintable(true);
    //
    //    GdkScreen *screen;
    //    GdkVisual *visual;
    //
    //    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    //    screen = gdk_screen_get_default();
    //    visual = gdk_screen_get_rgba_visual(screen);
    //
    //    if (visual != NULL && gdk_screen_is_composited(screen)) {
    //        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    //    }
    //    




    DockPanel::m_currentMoveIndex = -1;
    // m_location = Configuration::get_dockWindowLocation();

    initTime = 0;
    endPosition = 200.0;
    // DockPanel::m_DockWindow = nullptr;







    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();
    //    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
    //                     G_CALLBACK(DockPanel::on_window_opened), NULL);
    //
    //    g_signal_connect(wnckscreen, "window_closed",
    //                     G_CALLBACK(DockPanel::on_window_closed), NULL);

    //    g_signal_connect(wnckscreen, "active_window_changed",
    //                     G_CALLBACK(DockPanel::on_active_window_changed_callback), NULL);


    Glib::signal_timeout().connect(sigc::mem_fun(*this,
                                                 &DockPanel::on_timeoutDraw), DEF_FRAMERATE);

    // Launch timer every second
    // Glib::signal_timeout().connect(sigc::mem_fun(*this,
    //                                             &DockPanel::on_timeoutDraw), 1000);



}

/**
 * preInit load the attached icons and initializes variables, 
 * and create the popup menus
 * @param Gtk::Window*  window
 * @return return 0 is success or -1 is an error found
 */
int DockPanel::preInit(Gtk::Window* window)
{

    //this->m_DockWindow = (DockWindow*) window;

    //getDockWindowWidth(); //+180);//DockWindow::get_geometry().height; //this->


    //  m_cellheight = Configuration::get_CellHeight();
    //  m_cellwidth = Configuration::get_CellWidth();

    const char* filename = m_homeiconFilePath.c_str();
    DockItem* dockItem = new DockItem();
    try {

        int m_iconsize = Configuration::get_CellWidth() - ICON_CELL_WIDTH_MARGIN;
        dockItem->m_image = Gdk::Pixbuf::create_from_file(filename,
                                                          m_iconsize, m_iconsize, true);
    }
    catch (Glib::FileError fex) {
        g_critical("preInit: file %s could not be found!\n", filename);
        return -1;

    }
    catch (Gdk::PixbufError bex) {
        g_critical("preInit: file %s PixbufError!\n", filename);
        return -1;
    }

    dockItem->m_appname = _("Desktop");
    dockItem->m_realgroupname = _("Desktop");
    dockItem->m_dockitemtype = DockItemType::SingleDock;

    this->m_appUpdater.m_dockitems.insert(this->m_appUpdater.m_dockitems.begin(), dockItem);


    // testt separator
    /*
    dockItem = new DockItem();
    dockItem->m_dockitemtype = DockItemType::Separator;
    dockItem->m_isAttached = true;
    dockItem->m_width = 12;
    this->m_appUpdater.m_dockitems.push_back(dockItem);
     */

    DockWindow::update();
    //    if (m_AppWindow != nullptr) {
    //        ((AppWindow*)m_AppWindow)->update(true);
    //    }





}

DockPanel::~DockPanel(){ }

bool DockPanel::on_timeoutEasing()
{
    m_time++;
}

/** 
 * bool DockPanel::on_button_press_event(GdkEventButton *event)
 * 
 * handles Mouse button press : process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_button_press_event(GdkEventButton *event)
{

    if ((event->type == GDK_BUTTON_PRESS)) {

        m_currentMoveIndex = getIndex(event->x, event->y);

        // g_print("MDOWN %d\n", m_currentMoveIndex);
        //      m_mouseRightClick = false;
        /*
                // Set Drag and drop variables and Starts the timer
                if (event->button == 1 && !m_dragdropTimerSet && m_currentMoveIndex > 0) {

                    // remember the item we want to drag
                    m_dragdropItemIndex = m_currentMoveIndex;
                    int relativex = DockPosition::getDockItemRelativeMouseXPos(
                            (int) m_dockitems.size(), m_dragdropItemIndex,
                            m_cellwidth, (int) event->x);

                    // sets the relative item mouse coordinates         
                    m_dragdropMousePoint.set_x(relativex);
                    m_dragdropMousePoint.set_y((int) event->y);

                    // Variables are set 
                    m_dragdropTimerSet = true;
                    // Start the timer
                    m_dragdropMouseDown = true;
                    m_dragdropTimer.start();
                    // g_print("Drag timer starts\n");
                }
         */

        // remember the time to check it later 
        m_mouseclickEventTime = gtk_get_current_event_time();

        // Check if the event is a left button click.
        if (event->button == 1 && !m_mouseLeftButtonDown) {
            m_mouseLeftButtonDown = true;
            // The event has been handled.
            return true;
        }
        // Check if the event is a right button click.
        if (event->button == 3 && !m_mouseRightButtonDown) {
            m_mouseRightButtonDown = true;
            // The event has been handled.

            // Items 
            if (m_currentMoveIndex > 0) {
                DockItem *dockitem = m_appUpdater.m_dockitems.at(m_currentMoveIndex);
                dockitem->m_isAttached = true;
                dockitem->m_attachedIndex = m_currentMoveIndex;
                this->m_appUpdater.Save();
            }

            return true;
        }
    }
    return false;
}

/** 
 * bool DockPanel::on_button_release_event(GdkEventButton *event)
 * Returning TRUE means we handled the event, so the signal
 * emission should be stopped (don’t call any further callbacks
 * that may be connected). Return FALSE to continue invoking callbacks.
 * handles Mouse button released : process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_button_release_event(GdkEventButton *event)
{
    
    /*
    // Check if a item was drop
    if (m_dragdropMouseDown) {
        m_dragdropsStarts = false;
        m_dragdropTimerSet = false;
        m_dragdropMouseDown = false;
        m_dragDropWindow.Hide();
        m_dragdropTimer.stop();

        if (m_dragdropItemIndex != m_currentMoveIndex
                && m_currentMoveIndex > 0) {

            dropDockItem(event);

            m_dragdropItemIndex = -1;
        }
    }
     */

    // Taking Too Long To Release the mouse.
    // That is not a valid Click.
    if ((gtk_get_current_event_time() - m_mouseclickEventTime) > 200) {
        m_mouseLeftButtonDown = false;
        m_mouseRightButtonDown = false;
        return true;
    }

    if (!m_mouseIn)
        return true;

    if (m_mouseLeftButtonDown) {
        m_mouseLeftButtonDown = false;
        g_print("Click \n");
        this->ExecuteApp(event);
        //  SelectWindow(m_currentMoveIndex, event);
        return TRUE;
    }
    /*
        if (m_mouseRightButtonDown) {
            m_preview.hideMe();
            m_mouseRightClick = true;
            m_mouseRightButtonDown = false;

            // Items 
            if (m_currentMoveIndex > 0) {
                DockItem *dockitem = m_dockitems.at(m_currentMoveIndex);
                int isExitstWindows =
                        WindowControl::isExitstWindowsByDockItem(dockitem);
                bool isExitstActiveWindow =
                        WindowControl::isExitsActivetWindowByDockItem(dockitem);
                bool maximizedexistst =
                        WindowControl::isExistsUnMaximizedWindowsByDockItem(dockitem);


                m_MenuItemNewApp.set_label(_("Open new"));
                m_MenuItemNewApp.set_sensitive(true);
                if (dockitem->m_dockitemSesssionGrpId > 0) {

                    m_MenuItemNewApp.set_label(_("Configure"));
                    m_MenuItemNewApp.set_sensitive(true);

                }


                m_MenuItemMinimizedAll.set_sensitive(isExitstWindows > 0 && maximizedexistst);

                m_MenuItemMinimizedAllExceptActive.set_sensitive(isExitstWindows > 1
                        && maximizedexistst && isExitstActiveWindow);
                m_MenuItemUnMinimizedAll.set_sensitive(isExitstWindows);


                m_MenuItemCloseAllExceptActive.set_sensitive(isExitstWindows > 1 && isExitstActiveWindow);
                m_MenuItemCloseAll.set_sensitive(isExitstWindows);

                m_MenuItemAttach.set_sensitive(dockitem->m_isAttached == false);
                m_MenuItemDetach.set_sensitive(dockitem->m_isAttached &&
                        dockitem->m_items.size() == 0);


                if (!m_Menu_Popup.get_attach_widget())
                    m_Menu_Popup.attach_to_widget(*this);


                //m_HomeMenu_Popup.resize_children();
                m_Menu_Popup.set_halign(Gtk::Align::ALIGN_CENTER);
                m_Menu_Popup.popup(sigc::mem_fun(*this,
                        &DockPanel::on_popup_menu_position), 1, event->time);

                m_popupMenuOn = true;
                return true;
            }
        
            //Home
            if (m_currentMoveIndex == 0) {

                int wincount = WindowControl::windowscount();
                //int minimizedexitst = WindowControl::isExistsMinimizedWindows();
                int unminimized = WindowControl::unMinimizedWindowsCount();

                m_HomeCloseAllWindowsMenuItem.set_sensitive(wincount > 0);
                m_HomeCloseAllWindowsExceptActiveMenuItem.set_sensitive(wincount > 1);
                m_HomeMinimizeAllWindowsMenuItem.set_sensitive(unminimized > 0);
                m_HomeUnMinimizeAllWindowsMenuItem.set_sensitive(wincount > 0);

                m_HomeMinimizeAllWindowsExceptActiveMenuItem.set_sensitive(unminimized > 1);

                if (!m_HomeMenu_Popup.get_attach_widget())
                    m_HomeMenu_Popup.attach_to_widget(*this);

                //m_HomeMenu_Popup.resize_children();
                m_HomeMenu_Popup.set_halign(Gtk::Align::ALIGN_CENTER);
                m_HomeMenu_Popup.popup(sigc::mem_fun(*this,
                        &DockPanel::on_popup_homemenu_position), 1, event->time);

                m_popupMenuOn = true;

                return true;
            }

            return TRUE;
        }
     */
    return TRUE;
}

void DockPanel::ExecuteApp(GdkEventButton* event)
{
    if (m_currentMoveIndex < 1)
        return;

    DockItem * item = this->m_appUpdater.m_dockitems.at(m_currentMoveIndex);
    if (item->m_dockitemtype == DockItemType::Separator) {
        return;
    }

    if (item->m_items.size() == 0) {
        on_menuNew_event();
        return;
    }

    // TEST must be item->m_items.size() == 1
    // must be item->m_items.size() >1 // open preview
    if (item->m_items.size() >= 1) {
        DockItem* firstChild = item->m_items.at(0);
        WnckWindow *window = firstChild->m_window;

        auto ct = gtk_get_current_event_time();
        if (wnck_window_is_minimized(window)) {
            wnck_window_unminimize(window, ct);
            wnck_window_activate(window, ct);
            return;
        }
        if (wnck_window_is_active(window) == false) {
            wnck_window_unminimize(window, ct);
            wnck_window_activate(window, ct);
            return;
        }

        if (wnck_window_is_active(window) == true) {
            wnck_window_minimize(window);
            return;
        }

        if (wnck_window_is_minimized(window)) {
            wnck_window_unminimize(window, ct);
            wnck_window_activate(window, ct);
            return;
        }

    }
}

void DockPanel::on_menuNew_event()
{
    ;
    if (m_currentMoveIndex < 1)
        return;

    DockItem * item = this->m_appUpdater.m_dockitems.at(m_currentMoveIndex);

    if (item->m_dockitemSesssionGrpId > 0) {
        //  createSessionWindow();
        return;

    }

    // m_selectorAnimationItemIndex = m_currentMoveIndex;
    if (!Launcher::Launch(item->m_realgroupname)) {

        //   createLauncher(item);
    }


}

bool DockPanel::on_timeoutDraw()
{
    if (m_mouseIn) {
        Gtk::Widget::queue_draw();
    }
    return true;
}

bool DockPanel::canShow()
{
    //    if (this->m_mouseIn == false) {
    //        return false;
    //    }

    int x, y;
    Utilities::getMousePosition(x, y);

    switch (Configuration::get_dockWindowLocation())
    {
        case panel_locationType::TOP:
        {
            int startX = abs((DockWindow::get_geometry().width / 2) - (DockWindow::get_DockWindowWidth() / 2));
            int endX = startX + DockWindow::get_DockWindowWidth();

            if (x > startX && x < endX && y - 5 < Configuration::get_WindowDockMonitorMargin_Top()) {
                m_mouseIn = true;
                return true;
            }
            break;
        }
        case panel_locationType::BOTTOM:
        {
            int startX = abs((DockWindow::get_geometry().width / 2) - (DockWindow::get_DockWindowWidth() / 2));
            int endX = startX + DockWindow::get_DockWindowWidth();

            //g_print("%d %d\n", x, startX);
            if (x > startX && x < endX && y + 5 > (DockWindow::get_geometry().height - Configuration::get_WindowDockMonitorMargin_Bottom())) {
                m_mouseIn = true;
                return true;
            }
            break;
        }
        case panel_locationType::LEFT:
        {
            int startY = abs((DockWindow::get_geometry().height / 2) - (DockWindow::get_DockWindowHeight() / 2));
            int endY = startY + DockWindow::get_DockWindowHeight();

            if ((y > startY && y < endY) && (x <= 1 || x - 5 < Configuration::get_WindowDockMonitorMargin_Left())) {
                m_mouseIn = true;
                return true;
            }
            break;
        }
        case panel_locationType::RIGHT:
        {
            int startY = abs((DockWindow::get_geometry().height / 2) - (DockWindow::get_DockWindowHeight() / 2));
            int endY = startY + DockWindow::get_DockWindowHeight();

            if ((y > startY && y < endY) && (x + 5 > (DockWindow::get_geometry().width - Configuration::get_WindowDockMonitorMargin_Right()))) {
                m_mouseIn = true;
                return true;
            }
            break;
        }
    }


    return false;
}

/**
 * get the item index from mouse coordinates.
 * @param x
 * @param y
 * @return 
 */
int DockPanel::getIndex(int x, int y)
{
    int center = DockWindow::get_DockWindowWidth() / 2;
    Gdk::Point mouse(x, y);
    int result = -1;
    int idx = 0;
    int col = 0;


    switch (Configuration::get_dockWindowLocation())
    {
        case panel_locationType::TOP:
        case panel_locationType::BOTTOM:
        {
            if (Configuration::is_panelMode()) {
                //                col = center - (m_dockitems.size() * m_cellwidth) / 2;
                //                col -= (Configuration::get_separatorMargin() + m_cellwidth);
            }
            else {
                col = (Configuration::get_separatorMargin() / 2);

            }

            if (Configuration::get_HorizontalAlignment() == Horizontal_alignment_type::CENTER) {
                col = center - (this->get_dockItemsWidth() / 2) + (Configuration::get_separatorMargin() / 2);
            }

            for (auto item:this->m_appUpdater.m_dockitems) {
                if (mouse.get_x() >= col && mouse.get_x() <= (col + item->m_width)) {
                    result = idx;
                    break;
                }
                col += item->m_width + Configuration::get_separatorMargin();
                idx++;
            }
        }
            break;
        case panel_locationType::LEFT:
        case panel_locationType::RIGHT:
        {

            center = DockWindow::get_DockWindowHeight() / 2;
            col = (Configuration::get_separatorMargin() / 2);
            //            if (Configuration::is_panelMode()) {
            //                //col = center - (m_dockitems.size() * m_cellwidth) / 2;
            //                //col -= (Configuration::get_separatorMargin() + m_cellwidth);
            //                //col = (Configuration::get_separatorMargin() / 2); 
            //            }
            //            else {
            //
            //                
            //            }

            if (Configuration::get_HorizontalAlignment() == Horizontal_alignment_type::CENTER) {
                col = center - (this->get_dockItemsHeight() / 2) + (Configuration::get_separatorMargin() / 2);
            }

            //  g_print("...index = : %d\n", result);
            int height = 0;
            for (DockItem* item:this->m_appUpdater.m_dockitems) {
                height = item->m_height;
                if (item->m_dockitemtype == DockItemType::Separator) {
                    height = item->m_width;
                }

                if (mouse.get_y() >= col && mouse.get_y() <= col + height) {
                    result = idx;
                    // g_print("............y:%d  col:%d d:%d h:%d idx:%d\n", mouse.get_y(),
                    //          col, (col + height), height, idx);
                    // g_print(".............%d col: %d \n", mouse.get_y() , col );
                    break;
                }
                col += height + (Configuration::get_separatorMargin());
                idx++;
            }

        }
            break;
    }

    // g_print("%d %d \n", mouse.get_x(), result);
    return result;
}

/**
 * handles Mouse motion: process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param event
 * @return 
 */
bool DockPanel::on_motion_notify_event(GdkEventMotion * event)
{
    m_currentMoveIndex = this->getIndex(event->x, event->y);
    return false;
}

void DockPanel::RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
                                 double x, double y, double width, double height, double radius)
{
    // radius can be no larger than half our height or width
    radius = std::min(radius, std::min(width / 2, height / 2));
    cr->move_to(x + radius, y);
    cr->arc(x + width - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
    cr->arc(x + width - radius, y + height - radius, radius, 0, M_PI * .5);
    cr->arc(x + radius, y + height - radius, radius, M_PI * .5, M_PI);
    cr->arc(x + radius, y + radius, radius, M_PI, M_PI * 1.5);

}

unsigned int DockPanel::get_dockItemsWidthUntilIndex(int idx)
{
    unsigned int size = 0;
    int count = 0;
    for (DockItem* item:this->m_appUpdater.m_dockitems) {
        if (count == idx) {
            break;
        }
        size += item->m_width;

        count++;
    }

    return size;
}

unsigned int DockPanel::get_dockItemsHeightUntilIndex(int idx)
{
    unsigned int size = 0;
    int count = 0;
    for (DockItem* item:this->m_appUpdater.m_dockitems) {
        if (count == idx) {
            break;
        }
        if (item->m_dockitemtype == DockItemType::Separator) {
            size += item->m_width;
            count++;
            continue;
        }
        size += item->m_height;

        count++;
    }

    return size;
}


inline int DockPanel::get_dockItemsWidth()
{
    unsigned int size = 0;
    for (DockItem* item:this->m_appUpdater.m_dockitems) {
        size += item->m_width;
    }

    return size + (Configuration::get_separatorMargin() * this->m_appUpdater.m_dockitems.size());
}

inline int DockPanel::get_dockItemsHeight()
{
    unsigned int size = 0;
    for (DockItem* item:this->m_appUpdater.m_dockitems) {

        if (item->m_dockitemtype == DockItemType::Separator) {
            size += item->m_width;
            continue;
        }

        size += item->m_height;
    }

    return size + (Configuration::get_separatorMargin() * this->m_appUpdater.m_dockitems.size());
}

void DockPanel::get_dockItemPosition(DockItem* item, int &x1, int &y1, int &x2, int &y2, int &center, int i)
{
    int start = 0;
    int increment = 0;

    switch (Configuration::get_dockWindowLocation())
    {
        case panel_locationType::TOP:
        case panel_locationType::BOTTOM:
        {

            endPosition = DockWindow::get_DockWindowHeight();

            int itemsWidth = this->get_dockItemsWidth();
            center = DockWindow::get_DockWindowWidth() / 2;

            start = 0;
            if (Configuration::get_HorizontalAlignment() == Horizontal_alignment_type::CENTER) {
                start = center - (itemsWidth / 2);
            }

            increment = get_dockItemsWidthUntilIndex(i);
            increment += (i * Configuration::get_separatorMargin()) + Configuration::get_separatorMargin() / 2;

            x1 = start + increment;
            y1 = CELL_TOP_MARGIN;
            x2 = item->m_width;
            y2 = Configuration::get_CellHeight(); //m_cellheight;

        }
            break;
        case panel_locationType::LEFT:
        case panel_locationType::RIGHT:
        {

            endPosition = DockWindow::get_DockWindowWidth();

            int itemsHeight = this->get_dockItemsHeight();
            start = 0;
            if (Configuration::get_HorizontalAlignment() == Horizontal_alignment_type::CENTER) {
                center = DockWindow::get_DockWindowHeight() / 2;
                start = center - (itemsHeight / 2);
            }

            increment = get_dockItemsHeightUntilIndex(i);
            increment += (i * Configuration::get_separatorMargin()) + Configuration::get_separatorMargin() / 2;


            x1 = (DockWindow::get_DockWindowWidth() / 2) - (Configuration::get_CellWidth() / 2);

            //            if (Configuration::get_dockWindowLocation() == panel_locationType::LEFT) {
            //                // JUAN x1 = DockWindow::getDockWindowWidth() - Configuration::get_CellWidth() - 6;
            //
            //                x1 = DockWindow::get_DockWindowWidth() - Configuration::get_CellWidth() - 6;
            //            }

            y1 = start + increment;
            x2 = Configuration::get_CellWidth();
            y2 = Configuration::get_CellHeight(); //m_cellheight;

            if (item->m_dockitemtype == DockItemType::Separator) {
                y2 = 8;
            }
        }
            break;

        default:
            break;
    }
}

bool DockPanel::on_enter_notify_event(GdkEventCrossing * crossing_event)
{
    //  g_print("Mp_IN\n");
    m_mouseIn = true;
    return true;
}

bool DockPanel::on_leave_notify_event(GdkEventCrossing * crossing_event)
{
    // g_print("Mp_OUT\n");
    m_mouseIn = false;
    return true;
}

bool DockPanel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (Configuration::is_allowDraw() == false) {
        return true;
    }


    int currentPositionX = 0;
    int currentPositionY = 0;

    int x1, y1, x2, y2, center;
    this->get_dockItemPosition(this->m_appUpdater.m_dockitems.at(0), x1, y1, x2, y2, center, 0);

//    if (Configuration::is_autoHide()) {
//
//        if (!m_animate && !m_mouseIn && m_visible && m_timerStoped) {
//            m_Timer.start();
//            m_timerStoped = false;
//        }
//        if (!m_animate && m_mouseIn && !m_timerStoped) {
//            m_timerStoped = true;
//        }
//
//        if (canShow()) {
//            if (!m_animate && m_mouseIn && !m_visible) {
//                m_animate = true;
//                m_easing_duration = 4.f;
//            }
//        }
//
//        if (!m_animate && m_visible && !m_mouseIn && m_Timer.elapsed() > 1.5) {
//            m_Timer.stop();
//            m_timerStoped = true;
//            m_animate = true;
//            m_easing_duration = 4.f;
//        }
//
//        if (m_animate) {
//            auto endTime = initTime + m_easing_duration;
//            auto now = atime;
//            int currentPositionX = x1;
//            int currentPositionY = y1;
//            float startPosition = 0.f;
//
//            position = ofxeasing::map_clamp(now, initTime, endTime, 0,
//                                            endPosition, &ofxeasing::linear::easeIn);
//
//            switch (Configuration::get_dockWindowLocation())
//            {
//                case panel_locationType::TOP:
//                    currentPositionX = 0;
//
//                    if (m_visible) {
//                        startPosition = 0;
//                        currentPositionY = (int)startPosition - (int)position;
//                    }
//                    else {
//                        startPosition = -DockWindow::get_DockWindowHeight();
//                        currentPositionY = (int)startPosition + (int)position;
//                    }
//
//                    break;
//                case panel_locationType::BOTTOM:
//                    currentPositionX = 0;
//
//                    if (m_visible) {
//                        startPosition = 0;
//                        currentPositionY = (int)startPosition + (int)position;
//
//                    }
//                    else { // show
//
//                        startPosition = DockWindow::get_DockWindowHeight();
//                        currentPositionY = (int)startPosition - (int)position;
//                    }
//
//                    break;
//                case panel_locationType::LEFT:
//
//                    currentPositionY = 0;
//                    if (m_visible) {
//                        startPosition = 0;
//                        currentPositionX = (int)startPosition - (int)position;
//
//                    }
//                    else { // show
//                        startPosition = -DockWindow::get_DockWindowWidth();
//                        currentPositionX = (int)startPosition + (int)position;
//                    }
//
//                    break;
//                case panel_locationType::RIGHT:
//                    currentPositionY = 0;
//
//                    if (m_visible) {
//                        startPosition = 0;
//                        currentPositionX = startPosition + position;
//                    }
//                    else {
//                        startPosition = DockWindow::get_DockWindowWidth();
//                        currentPositionX = startPosition - position;
//                    }
//                    break;
//            }
//
//            if (m_animate) {
//                this->draw_Panel(cr, currentPositionX, currentPositionY);
//                this->draw_Items(cr, currentPositionX, currentPositionY);
//            }
//
//            if (atime < m_easing_duration) {
//                atime++;
//            }
//
//            if (position >= endPosition) {
//                initTime = 0;
//                atime = 0;
//                m_animate = false;
//                m_visible = !m_visible;
//
//                if (!m_visible) {
//                    DockWindow::removeStrut();
//                }
//                else {
//                    DockWindow::updateStrut();
//                }
//            }
//        }
//    }

    
        this->draw_Panel(cr, currentPositionX, currentPositionY);
        this->draw_Items(cr, currentPositionX, currentPositionY);


    return true;
}

/**
 * Draw the panel in all locations
 * @param cr cairo context
 * @param x current position x
 * @param y current position y
 */
void DockPanel::draw_Panel(const Cairo::RefPtr<Cairo::Context>& cr, int x, int y)
{
    if (Configuration::is_panelMode()) {

        cr->set_source_rgba(0.0, 0.0, 0.0, 0.5);
        cr->paint();
        //RoundedRectangle(cr, x, y, DockWindow::get_DockWindowWidth(), DockWindow::get_DockWindowHeight(), 6);
    }
    else {
        cr->set_source_rgba(0.0, 0.0, 175.8, 0.4);
        RoundedRectangle(cr, x, y, DockWindow::get_DockWindowWidth(), DockWindow::get_DockWindowHeight(), 6);
        cr->fill();
    }

}

void DockPanel::draw_Items(const Cairo::RefPtr<Cairo::Context>& cr, int currentposX, int currentposY)
{

    int x, y, width, height;
    int idx = 0;
    int center = 0;
    for (DockItem* item:this->m_appUpdater.m_dockitems) {
        if (item->m_dockitemtype == DockItemType::Separator) {
            idx++;
            continue;
        }

        this->get_dockItemPosition(item, x, y, width, height, center, idx);


        // Draw cells
        RoundedRectangle(cr, x + currentposX, y + currentposY, width, height, 3);
        cr->set_source_rgba(0.00, 0.50, 0.66, 1);
        cr->fill();

        cr->set_line_width(1.5);
        RoundedRectangle(cr, x + currentposX, y + currentposY, width, height, 3);
        cr->set_source_rgba(255.0, 255.0, 255.0, 1);
        cr->stroke();


        /* DEBUG
      cr->save();
      char buffer[50];
      sprintf(buffer, "%d", x + currentposX);

      cr->set_font_size(13);
      cr->move_to(x + currentposX, 13); //x + currentposX+100,y + currentposY);
      cr->show_text(buffer);
      cr->restore();
            
         */




        // Draw circles

        double radius = 2.0;
        int margin = 4;

        int iconsize = Configuration::get_CellWidth() - 8;
        int center = (Configuration::get_CellWidth() / 2);

        int circleStartX = center;

        int startX = center - (iconsize / 2);
        int startY = (Configuration::get_CellHeight() / 2) - (iconsize / 2);

        circleStartX += currentposX + x;

        startX += currentposX + x;
        startY += (currentposY + y) - 4;

        cr->set_source_rgb(1.0, 1.0, 1.0);

        int heightPos = y + currentposY + Configuration::get_CellHeight() - 6;

        if (item->m_items.size() == 1) {
            cr->arc(circleStartX, heightPos, radius, 0, 2 * M_PI);
        }
        else if (item->m_items.size() > 1) {
            cr->arc(circleStartX - margin, heightPos, radius, 0, 2 * M_PI);
            cr->arc(circleStartX + margin, heightPos, radius, 0, 2 * M_PI);
        }

        cr->fill();

        // Draw icons
        if (item->m_image != NULLPB) {
            auto icon = item->m_image->scale_simple(iconsize, iconsize, Gdk::INTERP_BILINEAR);
            Gdk::Cairo::set_source_pixbuf(cr, icon, startX, startY);

            cr->paint();
        }
        idx++;
    }

    // Selector
    if (m_currentMoveIndex != -1 && m_currentMoveIndex < this->m_appUpdater.m_dockitems.size()) {
        int x, y, width, height, center;
        DockItem* item = this->m_appUpdater.m_dockitems.at(m_currentMoveIndex);
        
        if (item->m_dockitemtype != DockItemType::Separator) {
            this->get_dockItemPosition(item, x, y, width, height, center, m_currentMoveIndex);
            cr->set_source_rgba(255.0, 255.0, 255.0, 0.4);
            RoundedRectangle(cr, x + currentposX, y + currentposY, width, height, 3);
            cr->fill();

            cr->set_line_width(2.5);
            cr->set_source_rgba(255.0, 255.0, 255.0, 1);
            RoundedRectangle(cr, x + currentposX, y + currentposY, width, height, 3);
            cr->stroke();
        }
    }

}