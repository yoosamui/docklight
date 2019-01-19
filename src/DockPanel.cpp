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

// static members
std::vector<DockItem*> DockPanel::m_dockitems;
int DockPanel::m_currentMoveIndex;
//Gtk::Window* DockPanel::m_AppWindow;
//DockWindow* DockPanel::m_DockWindow;

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
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                     G_CALLBACK(DockPanel::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
                     G_CALLBACK(DockPanel::on_window_closed), NULL);

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


    m_cellheight = Configuration::get_CellHeight();
    m_cellwidth = Configuration::get_CellWidth();

    const char* filename = m_homeiconFilePath.c_str();
    DockItem* dockItem = new DockItem();
    try {

        int m_iconsize = m_cellwidth - ICON_CELL_WIDTH_MARGIN;
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

    m_dockitems.insert(m_dockitems.begin(), dockItem);


    // testt separator

    dockItem = new DockItem();
    dockItem->m_dockitemtype = DockItemType::Separator;
    dockItem->m_width = 12;
    m_dockitems.push_back(dockItem);


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

bool DockPanel::on_timeoutDraw()
{
    Gtk::Widget::queue_draw();
    return true;
}

/**
 * Emitted when a new Wnck.Window is opened on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_opened(WnckScreen *screen, WnckWindow* window, gpointer data)
{
    Update(window, Window_action::OPEN);
}

/**
 * Emitted when a Wnck.Window is closed on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{

    // updateSessioWindow(window);
    Update(window, Window_action::CLOSE);
}

/**
 * void DockPanel::Update(WnckWindow* window, bool mode)
 * Update the items and handle the X Window events window_close and window_open.
 * @param window
 * @param actiontype
 */
void DockPanel::Update(WnckWindow* window, Window_action actiontype)
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
        for (auto item:m_dockitems) {
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

        //        if (m_AppWindow != nullptr) {
        //            ((AppWindow*)m_AppWindow)->update(true);
        //        }
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

                    DockWindow::update();
                    return;
                }

                // remove this item
                delete( item->m_items.at(0));
                m_dockitems.erase(m_dockitems.begin() + i);

                // if is not attached then it is at the end on the list.
                // we need to reset the index.
                m_currentMoveIndex = -1;

                DockWindow::update();
                return;
            }
            else {
                // search for the xid and remove the item
                int idx = 0;
                for (auto c:item->m_items) {
                    if (c->m_xid == xid) {
                        delete(c);
                        item->m_items.erase(item->m_items.begin() + idx);
                        DockWindow::update();
                        return;
                    }
                    idx++;
                }
            }
        }
    }


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
                col = center - (m_dockitems.size() * m_cellwidth) / 2;
                col -= (Configuration::get_separatorMargin() + m_cellwidth);
            }
            else {
                // col = (Configuration::get_CellWidth() / 2) + (Configuration::get_separatorMargin() / 2);
                col = center - (this->get_dockItemsWidth() / 2) +  (Configuration::get_separatorMargin() / 2);
                //col+= (DockWindow::get_dockWindowStartEndMargin()/2);

                //- (DockWindow::get_dockWindowStartEndMargin()/2);
                // g_print("%d\n", col);
            }

          //  g_print("...col = : %d\n", col);
            
            for (auto item:m_dockitems) {
                if (mouse.get_x() >= col && mouse.get_x() <= (col + item->m_width)) {
                 //   g_print(".............%d %d %d %d\n", mouse.get_x() , col , (col + item->m_width), idx);
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
            col = 0;
            if (Configuration::is_panelMode()) {
                col = center - (m_dockitems.size() * m_cellwidth) / 2;
                col -= (Configuration::get_separatorMargin() + m_cellwidth);
            }
            else {
                col = center - (this->get_dockItemsHeight() / 2) + (Configuration::get_separatorMargin() / 2);
            }

           //  g_print("...index = : %d\n", result);
            int height = 0;
            for (DockItem* item:m_dockitems) {
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
bool DockPanel::on_motion_notify_event(GdkEventMotion*event)
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
    for (DockItem* item:m_dockitems) {
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
    for (DockItem* item:m_dockitems) {
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

unsigned int DockPanel::get_dockItemsWidth()
{

    unsigned int size = 0;
    for (DockItem* item:m_dockitems) {
        size += item->m_width;
    }

    return size + (Configuration::get_separatorMargin() * m_dockitems.size())
            ; //+            Configuration::get_itemSize(); //Margin 
}

unsigned int DockPanel::get_dockItemsHeight()
{

    unsigned int size = 0;
    for (DockItem* item:m_dockitems) {

        if (item->m_dockitemtype == DockItemType::Separator) {
            size += item->m_width;
            continue;
        }
        size += item->m_height;
    }

    return size + (Configuration::get_separatorMargin() * m_dockitems.size());

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
            //JUAN center = DockWindow::getDockWindowWidth() / 2;
            center = DockWindow::get_DockWindowWidth() / 2;

            start = center - (itemsWidth / 2);

            increment = get_dockItemsWidthUntilIndex(i);
            increment += (i * Configuration::get_separatorMargin()) + Configuration::get_separatorMargin() / 2;

            x1 = start + increment;
            y1 = CELL_TOP_MARGIN;
            x2 = item->m_width;
            y2 = m_cellheight;

        }
            break;
        case panel_locationType::LEFT:
        case panel_locationType::RIGHT:
        {

            endPosition = DockWindow::get_DockWindowWidth();

            int itemsHeight = this->get_dockItemsHeight();

            center = DockWindow::get_DockWindowHeight() / 2;
            start = center - (itemsHeight / 2);

            increment = get_dockItemsHeightUntilIndex(i);
            increment += (i * Configuration::get_separatorMargin()) + Configuration::get_separatorMargin() / 2;

            x1 = 6;

            if (Configuration::get_dockWindowLocation() == panel_locationType::LEFT) {
                // JUAN x1 = DockWindow::getDockWindowWidth() - Configuration::get_CellWidth() - 6;

                x1 = DockWindow::get_DockWindowWidth() - Configuration::get_CellWidth() - 6;
            }

            y1 = start + increment;
            x2 = Configuration::get_CellWidth();
            y2 = m_cellheight;

            if (item->m_dockitemtype == DockItemType::Separator) {
                y2 = 8;
            }
        }
            break;

        default:
            break;
    }
}

bool DockPanel::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    //  g_print("Mp_IN\n");
    m_mouseIn = true;
    return true;
}

bool DockPanel::on_leave_notify_event(GdkEventCrossing* crossing_event)
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
    this->get_dockItemPosition(m_dockitems.at(0), x1, y1, x2, y2, center, 0);

    if (Configuration::is_autoHide()) {
        if (!m_animate && !m_mouseIn && m_visible && m_timerStoped) {
            m_Timer.start();
            m_timerStoped = false;
        }

        if (!m_animate && m_mouseIn && !m_visible) {
            m_animate = true;
            m_easing_duration = 4.f;


        }

        if (!m_animate && m_visible && !m_mouseIn && m_Timer.elapsed() > 1.5) {
            m_Timer.stop();

            m_animate = true;
            m_easing_duration = 4.f;
        }

        if (m_animate) {

            auto endTime = initTime + m_easing_duration;
            auto now = atime;
            int currentPositionX = x1;
            int currentPositionY = y1;
            float startPosition = 0.f;

            position = ofxeasing::map_clamp(now, initTime, endTime, 0,
                                            endPosition, &ofxeasing::linear::easeIn);

            switch (Configuration::get_dockWindowLocation())
            {
                case panel_locationType::TOP:
                    currentPositionX = 0;
                    if (m_visible) {
                        startPosition = 0; //DockWindow::getDockWindowHeight();
                        currentPositionY = (int)startPosition - (int)position;

                    }
                    else {


                        startPosition = -DockWindow::get_DockWindowHeight();

                        currentPositionY = (int)startPosition + (int)position;
                    }
                    break;
                case panel_locationType::BOTTOM:
                    currentPositionX = 0;
                    if (m_visible) {
                        startPosition = 0;
                        currentPositionY = (int)startPosition + (int)position;

                    }
                    else { // show

                        startPosition = DockWindow::get_DockWindowHeight();

                        currentPositionY = (int)startPosition - (int)position;
                    }

                    break;
                case panel_locationType::LEFT:

                    currentPositionY = 0;
                    if (m_visible) {
                        startPosition = 0;
                        currentPositionX = (int)startPosition - (int)position;

                    }
                    else { // show


                        startPosition = -DockWindow::get_DockWindowWidth();

                        currentPositionX = (int)startPosition + (int)position;
                    }

                    break;
                case panel_locationType::RIGHT:

                    currentPositionY = 0;
                    if (m_visible) {

                        startPosition = 0;
                        currentPositionX = startPosition + position;

                    }
                    else {

                        startPosition = DockWindow::get_DockWindowWidth();

                        currentPositionX = startPosition - position;
                    }
                    break;
            }

            this->draw_Panel(cr, currentPositionX, currentPositionY);
            this->draw_Items(cr, currentPositionX, currentPositionY);

            if (atime < m_easing_duration) {
                atime++;
            }

            if (position >= endPosition) {
                initTime = 0;
                atime = 0;
                m_animate = false;
                m_timerStoped = true;
                m_visible = !m_visible;

                if (!m_visible) {

                    DockWindow::removeStrut();
                }
                else {
                    DockWindow::updateStrut();
                }
            }
        }

    }

    if (!m_animate && m_visible) {
        this->draw_Panel(cr, currentPositionX, currentPositionY);
        this->draw_Items(cr, currentPositionX, currentPositionY);



    }

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

    cr->set_source_rgba(0.0, 0.0, 175.8, 0.4);
    RoundedRectangle(cr, x, y, DockWindow::get_DockWindowWidth(), DockWindow::get_DockWindowHeight(), 6);
    cr->fill();
}

void DockPanel::draw_Items(const Cairo::RefPtr<Cairo::Context>& cr, int currentposX, int currentposY)
{

    int x, y, width, height;
    int idx = 0;
    int center = 0;
    for (DockItem* item:m_dockitems) {
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
    if (m_currentMoveIndex != -1) {
        int x, y, width, height, center;
        DockItem* item = m_dockitems.at(m_currentMoveIndex);
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