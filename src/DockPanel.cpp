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
//#include <gtkmm/messagedialog.h>
//#include <gdkmm/cursor.h>
//#include <limits.h>
//#include <math.h>
//#include <fstream>
//#include <iostream>
//#include <cairo/cairo.h>

// static members
int DockPanel::m_currentMoveIndex;

/*
 * this class is the main dock renderer.
 *
 */
DockPanel::DockPanel():
    m_homeiconFilePath(Utilities::getExecPath(DEF_ICONNAME))
{

    // Set event masks 
    add_events(Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
            Gdk::FOCUS_CHANGE_MASK |
            Gdk::ENTER_NOTIFY_MASK |
            Gdk::LEAVE_NOTIFY_MASK |
            Gdk::POINTER_MOTION_MASK);

    // set the static member to the default value -1;
    DockPanel::m_currentMoveIndex = -1;

    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();
    
    // set the required signals handlers
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
            G_CALLBACK(DockPanel::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
            G_CALLBACK(DockPanel::on_window_closed), NULL);

    //    g_signal_connect(wnckscreen, "active_window_changed",
    //                     G_CALLBACK(DockPanel::on_active_window_changed_callback), NULL);


    Glib::signal_timeout().connect(sigc::mem_fun(*this, &DockPanel::on_timeoutDraw), DEF_FRAMERATE);
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
/*
 *
 *
 */
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
 * calculate the item index from mouse coordinates.
 * @param x
 * @param y
 * @return the item index or -1 if the item could not be found.
 */
inline int DockPanel::getIndex(const int& mouseX, const int& mouseY)
{
    Gdk::Point mouse(mouseX, mouseY);
    int idx = 0;
    int x = DockWindow::get_dockWindowStartEndMargin() / 2;
    int y = x;

    if (DockWindow::is_Horizontal())  {
        for (auto item : this->m_appUpdater.m_dockitems) {
            if (mouse.get_x() >= x && mouse.get_x() <= x + item->m_width)                     {
                return idx;
            }

            x += item->m_width + Configuration::get_separatorMargin();
            idx++;
        }
    } else  {   
        int height;
        for (DockItem* item : this->m_appUpdater.m_dockitems) {
            height = item->m_dockitemtype == DockItemType::Separator ? item->m_width : item->m_height;

            if (mouse.get_y() >= y && mouse.get_y() <= y + height) {
                return idx;
            }

            y += height + Configuration::get_separatorMargin();
            idx++;
        }
    }

    return -1;
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
 * The result width will resize the dockwindow width.
 * referenced by DockWindow.
 * @return The width of the dockwindow.
 */
guint DockPanel::get_dockItemsWidth()
{
    guint size = DockWindow::get_dockWindowStartEndMargin();
    for (DockItem* item : this->m_appUpdater.m_dockitems) {
        size += item->m_width + Configuration::get_separatorMargin();
    }

    return size - Configuration::get_separatorMargin();
}

/**
 * Compute the height of all items
 * The result width will resize the dockwindow height.
 * referenced by DockWindow.
 * @return the height of the dockwindow.
 */
guint DockPanel::get_dockItemsHeight()
{
    guint size = DockWindow::get_dockWindowStartEndMargin();
    for (DockItem* item : this->m_appUpdater.m_dockitems) {
        if (item->m_dockitemtype == DockItemType::Separator) {
            size += item->m_width;
            continue;
        }

        size += item->m_height + Configuration::get_separatorMargin();
    }

    return size - Configuration::get_separatorMargin();
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

/**
 * Calculate the draw position for all items.
 */
inline void DockPanel::get_ItemPosition(const DockItemType dockType, int& x, int& y, int& width, int& height)
{
    static int nextsize = 0;

    if (DockWindow::is_Horizontal())
    {
        if (x == 0 )
        {
            y = (DockWindow::get_DockWindowHeight() / 2) - Configuration::get_CellHeight() / 2;
            x = DockWindow::get_dockWindowStartEndMargin() / 2;
            nextsize = width;

            return;
        }

        // if the item is a separator the wisth is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        if( dockType == DockItemType::Separator)
        {
            x +=  nextsize + Configuration::get_separatorMargin();
            nextsize = width;

            return;
        }

        x +=  nextsize + Configuration::get_separatorMargin();
        nextsize = width;
    }
    else
    {
        if (y == 0 )
        {
            x = (DockWindow::get_DockWindowWidth() / 2) - Configuration::get_CellWidth() / 2;
            y = DockWindow::get_dockWindowStartEndMargin() / 2;
            nextsize = height;

            return;
        }

        // if the item is a separator the wisth is probably not equal.
        // in this case wie remeber the size for use it in the next item.
        if( dockType == DockItemType::Separator)
        {
            y +=  nextsize + Configuration::get_separatorMargin();
            height = nextsize = width;
            width = Configuration::get_CellWidth();

            return;
        }

        y +=  nextsize + Configuration::get_separatorMargin();
        nextsize = height;

    }
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

    int y = 0;//CELL_TOP_MARGIN;
    int x = 0; //startPos = DockWindow::get_dockWindowStartEndMargin() / 2;
    int width = 0;
    int height = 0;
    for (DockItem* item:this->m_appUpdater.m_dockitems) {
        
        width = item->m_width;
        height = item->m_height;

        this->get_ItemPosition(item->m_dockitemtype, x, y, width, height);

        // Draw cells
        cr->set_source_rgba(0.00, 0.50, 0.66, 1);
        RoundedRectangle(cr, x, y, width, height, 3);
        cr->fill();

        cr->set_line_width(1.5);
        RoundedRectangle(cr, x, y, width, height, 3);
        cr->set_source_rgba(255.0, 255.0, 255.0, 1);
        cr->stroke();


        // Draw dots and icons if the item is not a user separator
        if (item->m_dockitemtype != DockItemType::Separator) {
            // Dots
            center = (width / 2);
            if (item->m_items.size() > 0) {
                cr->set_source_rgb(1.0, 1.0, 1.0);
                if (item->m_items.size() == 1) {
                    cr->arc(x + center, y + height - 5, 2.0, 0, 2 * M_PI);
                }
                else if (item->m_items.size() > 1) {
                    cr->arc(x + center - 4, y + height - 5, 2.0, 0, 2 * M_PI);
                    cr->arc(x + center + 4, y + height - 5, 2.0, 0, 2 * M_PI);
                }
                cr->fill();
            }
            // icons
            if (item->m_image != NULLPB) {
                iconsize = width - 8;

                //TODO: avoid scale
                auto icon = item->m_image->scale_simple(iconsize, iconsize, Gdk::INTERP_BILINEAR);
                Gdk::Cairo::set_source_pixbuf(cr, icon, x + center - iconsize / 2, y + 1);
                cr->paint();
            }
        }

        // Selector
        if (idx == m_currentMoveIndex) {
            cr->set_source_rgba(255.0, 255.0, 255.0, 0.4);
            RoundedRectangle(cr, x, y, width, height, 3);
            cr->fill();

            cr->set_line_width(2.5);
            cr->set_source_rgba(255.0, 255.0, 255.0, 1);
            RoundedRectangle(cr, x, y, width, height, 3);
            cr->stroke();
        }
        
        idx++;
    }


    // Draw the app title
   // draw_Title(cr);
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
