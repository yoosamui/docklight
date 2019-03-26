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
#include "DockItemPositions.h"
#include "Launcher.h"
#include "Animations.h"
#include "AppWindow.h"
#include "DockWindow.h"
#include "WnckHandler.h"

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


    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                     G_CALLBACK(DockPanel::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
                     G_CALLBACK(DockPanel::on_window_closed), NULL);


}

/**
 * preInit load the attached icons and initializes variables, 
 * and create the popup menus
 * @param Gtk::Window*  window
 * @return return 0 is success or -1 is an error found
 */
int DockPanel::preInit(Gtk::Window* window)
{

    // this->m_AppWindow = window;
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
static bool m_forceDraw = false;

/**
 * Emitted when a new Wnck.Window is opened on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_opened(WnckScreen *screen, WnckWindow* window, gpointer data)
{
    m_forceDraw = true;
    // force our program to redraw the entire clock.
    //    auto win = m_AppWindow;
    //    if (win)
    //    {
    //        Gdk::Rectangle r(0, 0, DockWindow::get_DockWindowWidth(), DockWindow::get_DockWindowHeight());
    //        m_AppWindow->dr
    //        win->invalidate_rect(r, false);
    //    }
    //gtk_widget_queue_draw_area(m_AppWindow,0,0,0,0) ;
}

/**
 * Emitted when a Wnck.Window is closed on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    m_forceDraw = true;
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

bool DockPanel::on_scroll_event(GdkEventScroll * e)
{
    int index = m_currentMoveIndex;
    if (index == -1 || index == 0)
        return true;

    DockItem * item = this->m_appUpdater.m_dockitems.at(index)->GetNext();
    if (item == nullptr)
        return true;

    WnckWindow * window = item->m_window;
    if ((int)e->delta_y == (int)1) {
        WnckHandler::ActivateWindow(window);
    }
    else if ((int)e->delta_y == (int) - 1) {
        WnckHandler::ActivateWindow(window);
    }

    // Event has been handled
    return true;
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
        WnckWindow *window = nullptr;
        window = WnckHandler::get_ActiveWindowIfAny(item);
        if (window == nullptr) {
            DockItem* firstChild = item->m_items.at(0);
            window = firstChild->m_window;
        }

        WnckHandler::ActivateWindow(window);
    }
}

DockItem* DockPanel::getCurrentItem()
{
    if (m_currentMoveIndex < 0 || m_currentMoveIndex > this->m_appUpdater.m_dockitems.size()) {
        return nullptr;
    }

    return this->m_appUpdater.m_dockitems.at(m_currentMoveIndex);
}

void DockPanel::AppRunAnimationLauncherCompleted()
{
    m_AppRunThreadLauncher->detach();
    delete m_AppRunThreadLauncher;
    m_AppRunThreadLauncher = nullptr;
}

void DockPanel::on_menuNew_event()
{

    if (m_currentMoveIndex < 1 || m_currentMoveIndex > this->m_appUpdater.m_dockitems.size()) {
        return;
    }

    DockItem* item = this->m_appUpdater.m_dockitems.at(m_currentMoveIndex);

    if (item->m_dockitemSesssionGrpId > 0) {
        //  createSessionWindow();
        return;

    }

    // start run animation
    if (m_AppRunThreadLauncher == nullptr) {
        Glib::RefPtr<Gdk::Pixbuf> m_image = item->m_image;
        m_AppRunThreadLauncher = new std::thread([this, m_image]{
            m_AppRunAnimation.start(*this, m_image);
        });
    }

    // m_selectorAnimationItemIndex = m_currentMoveIndex;
    if (!Launcher::Launch(item->m_realgroupname)) {

        //   createLauncher(item);
    }



}

void DockPanel::update()
{
    Gtk::Widget::queue_draw();
}

bool DockPanel::on_timeoutDraw()
{
    if (m_mouseIn || m_forceDraw) {
        Gtk::Widget::queue_draw();
        m_forceDraw = false;
    }
    return true;
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



    int startPostY = 0;
    int startPostX = DockWindow::get_dockWindowStartEndMargin() / 2;

    switch (Configuration::get_dockWindowLocation())
    {
        case panel_locationType::TOP:
        case panel_locationType::BOTTOM:
        {
            for (auto item:this->m_appUpdater.m_dockitems) {
                if (mouse.get_x() >= startPostX && mouse.get_x() <= startPostX + item->m_width) {
                    return idx;
                }

                startPostX += item->m_width + Configuration::get_separatorMargin();
                idx++;
            }

            return -1;
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

//obsolete use DockItemPosition instead

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
//obsolete use DockItemPosition instead

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

/**
 * Compute the width of all items.
 * @return The width of all items.
 */
gint DockPanel::get_dockItemsWidth()
{
    unsigned int size = 0;
    for (DockItem* item:this->m_appUpdater.m_dockitems) {
        //        if (item->m_dockitemtype == DockItemType::Separator) {
        //            size += item->m_width;
        //            continue;
        //        }

        size += item->m_width + Configuration::get_separatorMargin();
    }

    return size - Configuration::get_separatorMargin();
}

gint DockPanel::get_dockItemsHeight()
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

    m_titlewindow.hide();
    m_infowindow.hide();

    return true;
}

bool DockPanel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (Configuration::is_allowDraw() == false) {
        return true;
    }

    this->draw_Panel(cr);
    this->draw_Items(cr);

    return true;
}

/**
 * Draw the panel in all locations
 * @param cr cairo context
 * @param x current position x
 * @param y current position y
 */
void DockPanel::draw_Panel(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (Configuration::is_panelMode()) {

        cr->set_source_rgba(0.0, 0.0, 0.0, 0.8);
        cr->paint();
        //RoundedRectangle(cr, x, y, DockWindow::get_DockWindowWidth(), DockWindow::get_DockWindowHeight(), 6);
    }
    else {
        cr->set_source_rgba(0.0, 0.0, 175.8, 0.4);
        RoundedRectangle(cr, 0, 0, DockWindow::get_DockWindowWidth(), DockWindow::get_DockWindowHeight(), 6);
        cr->fill();
    }

}

/**
 * Render all dock items
 * @param cr cairo context
 */
void DockPanel::draw_Items(const Cairo::RefPtr<Cairo::Context>& cr)
{
    int idx = 0;
    int center = 0;
    int iconsize = 0;
    int selectorCurrentPos = -1;
    int y = CELL_TOP_MARGIN;
    int startPos = DockWindow::get_dockWindowStartEndMargin() / 2;
    
    for (DockItem* item : this->m_appUpdater.m_dockitems) {
        // Draw cells
        cr->set_source_rgba(0.00, 0.50, 0.66, 1);
        RoundedRectangle(cr, startPos, y, item->m_width, item->m_height, 3);
        cr->fill();
        cr->set_line_width(1.5);
        RoundedRectangle(cr, startPos, y, item->m_width, item->m_height, 3);
        cr->set_source_rgba(255.0, 255.0, 255.0, 1);
        cr->stroke();

        // Draw dots and icons if the item is not a user separator
        if (item->m_dockitemtype != DockItemType::Separator) {
            // Dots
            center = (item->m_width / 2);
            cr->set_source_rgb(1.0, 1.0, 1.0);
            if (item->m_items.size() == 1) {
                cr->arc(startPos + center, item->m_height + 3, 2.0, 0, 2 * M_PI);
            }
            else if (item->m_items.size() > 1) {
                cr->arc(startPos + center - 3, item->m_height + 3, 2.0, 0, 2 * M_PI);
                cr->arc(startPos + center + 3, item->m_height + 3, 2.0, 0, 2 * M_PI);
            }
            cr->fill();

            // icons
            if (item->m_image != NULLPB) {
                iconsize = item->m_width - 8;
                auto icon = item->m_image->scale_simple(iconsize, iconsize, Gdk::INTERP_BILINEAR);
                Gdk::Cairo::set_source_pixbuf(cr, icon, startPos + center - iconsize / 2 , y + 1);
                cr->paint();
            }
        }

        // save the selected current index position
        if (idx == m_currentMoveIndex) {
            selectorCurrentPos = startPos;
        }

        // calculate next position
        startPos += item->m_width + Configuration::get_separatorMargin();
        idx++;
    }

    // Selector
    if (m_currentMoveIndex != -1 && m_currentMoveIndex < this->m_appUpdater.m_dockitems.size()) {
        DockItem* item = this->m_appUpdater.m_dockitems.at(m_currentMoveIndex);
        cr->set_source_rgba(255.0, 255.0, 255.0, 0.4);
        RoundedRectangle(cr, selectorCurrentPos, y, item->m_width, item->m_height, 3);
        cr->fill();
        cr->set_line_width(2.5);
        cr->set_source_rgba(255.0, 255.0, 255.0, 1);
        RoundedRectangle(cr, selectorCurrentPos, y, item->m_width, item->m_height, 3);
        cr->stroke();
    }

    // Draw the app title
    draw_Title(cr);
}

void DockPanel::draw_Title(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (m_currentMoveIndex < 0 || m_currentMoveIndex > this->m_appUpdater.m_dockitems.size()) {
        return;
    }


    // title window
    if (m_titleItemOldindex != m_currentMoveIndex) {
        m_titleItemOldindex = m_currentMoveIndex;
        m_titleElapsedSeconds = 0;
        m_titleTimer.start();
        m_titleShow = false;

        m_titlewindow.hide();
        m_infowindow.hide();
        //            if (m_previewWindowActive)
        //                m_preview.hideMe();

    }

    if (m_titleItemOldindex == m_currentMoveIndex) {
        if (m_titleElapsedSeconds > 0.3 && m_titleShow == false /* && !m_previewWindowActive*/) {

            DockItem* item = this->getCurrentItem();
            if (item == nullptr)
                return;

            std::string title = item->getTitle();

            //            int xx , yy;
            //            m_titlewindow.get_position(xx,yy);
            //            
            //            title += " " + std::to_string(xx);
            //            

            if (item->m_items.size() > 1) {
                char buff[NAME_MAX];
                sprintf(buff, "%s (%d)", title.c_str(), (int)item->m_items.size());
                title = buff;
            }

            m_titlewindow.setText(title);
            int centerpos = DockItemPositions::getStartPos(*item,
                                                           this->m_appUpdater.m_dockitems.size(),
                                                           m_currentMoveIndex,
                                                           m_titlewindow.get_width());




            //                int y = MonitorGeometry::getScreenHeight() - m_titlewindow.get_height() -
            //                        MonitorGeometry::getStrutHeight();
            //                
            int y = DockWindow::get_geometry().height - this->get_height() - m_titlewindow.get_height();


            m_titlewindow.move(centerpos, y);
            m_titleShow = true;

        }

        m_titleElapsedSeconds = m_titleTimer.elapsed();
    }


}